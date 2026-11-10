#include <Arduino.h>
#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <stdio.h>

const byte SO_HANG_PHIM = 4;
const byte SO_COT_PHIM = 4;
const byte SO_COT_LCD = 20;
const byte SO_HANG_LCD = 4;
const byte SO_CHU_SO_TOI_DA = 4;
const byte KICH_THUOC_BO_DEM = 2 * SO_CHU_SO_TOI_DA + 2;
const byte CHAN_NUT_TRAI = 6;
const byte CHAN_NUT_PHAI = 7;
const unsigned long THOI_GIAN_CHONG_DOI = 30;

char maBanPhim[SO_HANG_PHIM][SO_COT_PHIM] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte chanHangPhim[SO_HANG_PHIM] = {8, 9, 10, 11};
byte chanCotPhim[SO_COT_PHIM] = {2, 3, 4, 5};

Keypad banPhim(makeKeymap(maBanPhim), chanHangPhim, chanCotPhim,
               SO_HANG_PHIM, SO_COT_PHIM);
LiquidCrystal_I2C manHinhLCD(0x27, SO_COT_LCD, SO_HANG_LCD);

byte diaChiLCD = 0;
char boDemBieuThuc[KICH_THUOC_BO_DEM];
char *conTroGhiKyTu = boDemBieuThuc;
char *conTroChinhSua = boDemBieuThuc;
char *conTroToanTu = nullptr;
bool daTinhXong = false;

struct NutNhan {
  byte chan;
  bool mucDocTruoc;
  bool mucOnDinh;
  unsigned long thoiDiemThayDoi;
};

NutNhan nutTrai = {CHAN_NUT_TRAI, HIGH, HIGH, 0};
NutNhan nutPhai = {CHAN_NUT_PHAI, HIGH, HIGH, 0};

bool coThietBiI2C(byte diaChi) {
  Wire.beginTransmission(diaChi);
  return Wire.endTransmission() == 0;
}

byte timDiaChiLCD() {
  const byte diaChiDau[2] = {0x20, 0x38};
  for (byte nhom = 0; nhom < 2; ++nhom) {
    for (byte diaChi = diaChiDau[nhom];
         diaChi < diaChiDau[nhom] + 8; ++diaChi) {
      if (coThietBiI2C(diaChi)) return diaChi;
    }
  }
  return 0;
}

bool nutVuaDuocNhan(NutNhan &nut) {
  bool mucDangDoc = digitalRead(nut.chan);
  if (mucDangDoc != nut.mucDocTruoc) {
    nut.mucDocTruoc = mucDangDoc;
    nut.thoiDiemThayDoi = millis();
  }
  if (millis() - nut.thoiDiemThayDoi >= THOI_GIAN_CHONG_DOI &&
      mucDangDoc != nut.mucOnDinh) {
    nut.mucOnDinh = mucDangDoc;
    return mucDangDoc == LOW;
  }
  return false;
}

void xoaDongLCD(byte hang) {
  manHinhLCD.setCursor(0, hang);
  for (byte cot = 0; cot < SO_COT_LCD; ++cot) {
    manHinhLCD.write(' ');
  }
  manHinhLCD.setCursor(0, hang);
}

void inDongFlash(byte hang, const __FlashStringHelper *noiDung) {
  xoaDongLCD(hang);
  manHinhLCD.print(noiDung);
}

void inDongSRAM(byte hang, const char *conTroChuoi) {
  xoaDongLCD(hang);
  manHinhLCD.print(conTroChuoi);
}

void datConTroLenLCD() {
  byte viTri = (byte)(conTroChinhSua - boDemBieuThuc);
  manHinhLCD.setCursor(4 + viTri, 1);
}

void hienThiBieuThuc() {
  xoaDongLCD(1);
  manHinhLCD.print(F("BT: "));
  manHinhLCD.print(boDemBieuThuc);
  datConTroLenLCD();
}

void capNhatConTroToanTu() {
  conTroToanTu = nullptr;
  for (char *conTro = boDemBieuThuc; conTro < conTroGhiKyTu; ++conTro) {
    if (*conTro == '+' || *conTro == '-' ||
        *conTro == '*' || *conTro == '/') {
      conTroToanTu = conTro;
      return;
    }
  }
}

void xoaBieuThuc() {
  conTroGhiKyTu = boDemBieuThuc;
  conTroChinhSua = boDemBieuThuc;
  conTroToanTu = nullptr;
  *conTroGhiKyTu = '\0';
  daTinhXong = false;
  hienThiBieuThuc();
  inDongFlash(2, F("NHAP BIEU THUC"));
}

void diChuyenTrai() {
  if (conTroChinhSua > boDemBieuThuc) {
    --conTroChinhSua;
    daTinhXong = false;
    inDongFlash(2, F("DANG SUA BIEU THUC"));
  }
  datConTroLenLCD();
}

void diChuyenPhai() {
  if (conTroChinhSua < conTroGhiKyTu) {
    ++conTroChinhSua;
    daTinhXong = false;
    inDongFlash(2, F("DANG SUA BIEU THUC"));
  }
  datConTroLenLCD();
}

void xoaMotKyTu() {
  if (conTroChinhSua == boDemBieuThuc) {
    inDongFlash(2, F("KHONG CO KY TU BEN TRAI"));
    return;
  }

  char *viTriCanXoa = conTroChinhSua - 1;
  if ((*viTriCanXoa == '+' || *viTriCanXoa == '-' ||
       *viTriCanXoa == '*' || *viTriCanXoa == '/') &&
      conTroGhiKyTu - boDemBieuThuc - 1 > SO_CHU_SO_TOI_DA) {
    inDongFlash(2, F("XOA SO TRUOC KHI XOA"));
    return;
  }

  for (char *conTro = viTriCanXoa; conTro < conTroGhiKyTu; ++conTro) {
    *conTro = *(conTro + 1);
  }
  --conTroGhiKyTu;
  --conTroChinhSua;
  capNhatConTroToanTu();
  daTinhXong = false;
  hienThiBieuThuc();
  inDongFlash(2, F("DA XOA 1 KY TU"));
}

bool chenKyTuVaoBoDem(char kyTu) {
  if (conTroGhiKyTu >= boDemBieuThuc + KICH_THUOC_BO_DEM - 1) {
    inDongFlash(2, F("LOI: BO DEM DAY"));
    return false;
  }

  char *conTro = conTroGhiKyTu;
  while (true) {
    *(conTro + 1) = *conTro;
    if (conTro == conTroChinhSua) break;
    --conTro;
  }
  *conTroChinhSua = kyTu;
  ++conTroGhiKyTu;
  ++conTroChinhSua;
  capNhatConTroToanTu();
  daTinhXong = false;
  hienThiBieuThuc();
  return true;
}

void nhapChuSo(char chuSo) {
  if (daTinhXong) xoaBieuThuc();

  char *dauSo = boDemBieuThuc;
  char *cuoiSo = conTroGhiKyTu;
  if (conTroToanTu != nullptr) {
    if (conTroChinhSua <= conTroToanTu) {
      cuoiSo = conTroToanTu;
    } else {
      dauSo = conTroToanTu + 1;
    }
  }

  if (cuoiSo - dauSo >= SO_CHU_SO_TOI_DA) {
    inDongFlash(2, F("TOI DA 4 CHU SO"));
    return;
  }
  if (chenKyTuVaoBoDem(chuSo)) {
    inDongFlash(2, F("NHAN # DE TINH"));
  }
}

void nhapToanTu(char toanTu) {
  if (daTinhXong) xoaBieuThuc();
  if (conTroChinhSua == boDemBieuThuc) {
    inDongFlash(2, F("NHAP SO THU NHAT"));
    return;
  }
  if (conTroToanTu != nullptr) {
    inDongFlash(2, F("CHI DUOC 1 TOAN TU"));
    return;
  }
  if (chenKyTuVaoBoDem(toanTu)) {
    inDongFlash(2, F("NHAP SO THU HAI"));
  }
}

const char *docSoNguyen(const char *conTroDocKyTu, int32_t *conTroGiaTriSo) {
  int32_t soDangDoc = 0;
  byte soChuSoDaDoc = 0;
  while (soChuSoDaDoc < SO_CHU_SO_TOI_DA &&
         *conTroDocKyTu >= '0' && *conTroDocKyTu <= '9') {
    soDangDoc = soDangDoc * 10 + (*conTroDocKyTu - '0');
    ++conTroDocKyTu;
    ++soChuSoDaDoc;
  }
  *conTroGiaTriSo = soDangDoc;
  return conTroDocKyTu;
}

void tinhBieuThuc() {
  int32_t soThuNhat = 0;
  int32_t soThuHai = 0;
  const char *conTroViTriToanTu = docSoNguyen(boDemBieuThuc, &soThuNhat);
  if (conTroViTriToanTu == boDemBieuThuc || *conTroViTriToanTu == '\0') {
    inDongFlash(2, F("SAI BIEU THUC"));
    return;
  }

  char toanTu = *conTroViTriToanTu;
  if (toanTu != '+' && toanTu != '-' &&
      toanTu != '*' && toanTu != '/') {
    inDongFlash(2, F("TOAN TU KHONG HOP LE"));
    return;
  }

  const char *conTroBatDauSoHai = conTroViTriToanTu + 1;
  const char *conTroKetThuc = docSoNguyen(conTroBatDauSoHai, &soThuHai);
  if (conTroKetThuc == conTroBatDauSoHai || *conTroKetThuc != '\0') {
    inDongFlash(2, F("SAI BIEU THUC"));
    return;
  }

  char chuoiKetQua[SO_COT_LCD + 1];
  int32_t ketQua = 0;
  switch (toanTu) {
    case '+': ketQua = soThuNhat + soThuHai; break;
    case '-': ketQua = soThuNhat - soThuHai; break;
    case '*': ketQua = soThuNhat * soThuHai; break;
    case '/':
      if (soThuHai == 0) {
        inDongFlash(2, F("LOI: CHIA CHO 0"));
        return;
      }
      snprintf(chuoiKetQua, sizeof(chuoiKetQua), "KQ: %ld DU: %ld",
               (long)(soThuNhat / soThuHai), (long)(soThuNhat % soThuHai));
      inDongSRAM(2, chuoiKetQua);
      daTinhXong = true;
      return;
  }

  snprintf(chuoiKetQua, sizeof(chuoiKetQua), "KQ: %ld", (long)ketQua);
  inDongSRAM(2, chuoiKetQua);
  daTinhXong = true;
}

char doiPhimSangToanTu(char phim) {
  switch (phim) {
    case 'A': return '+';
    case 'B': return '-';
    case 'C': return '*';
    case 'D': return '/';
    default: return '\0';
  }
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
  pinMode(CHAN_NUT_TRAI, INPUT_PULLUP);
  pinMode(CHAN_NUT_PHAI, INPUT_PULLUP);
  banPhim.setDebounceTime(20);

  diaChiLCD = timDiaChiLCD();
  if (diaChiLCD == 0) {
    Serial.println(F("KHONG TIM THAY LCD I2C: KIEM TRA A4/A5"));
    return;
  }
  manHinhLCD = LiquidCrystal_I2C(diaChiLCD, SO_COT_LCD, SO_HANG_LCD);
  manHinhLCD.init();
  manHinhLCD.backlight();
  manHinhLCD.cursor();
  manHinhLCD.blink();
  Serial.print(F("LCD I2C: 0x"));
  Serial.println(diaChiLCD, HEX);

  inDongFlash(0, F("A:+ B:- C:* D:/"));
  inDongFlash(3, F("D6:< D7:> *=XOA #=KQ"));
  xoaBieuThuc();
  datConTroLenLCD();
}

void loop() {
  if (diaChiLCD == 0) return;

  if (nutVuaDuocNhan(nutTrai)) diChuyenTrai();
  if (nutVuaDuocNhan(nutPhai)) diChuyenPhai();

  char phimDuocNhan = banPhim.getKey();
  if (phimDuocNhan == NO_KEY) return;

  if (phimDuocNhan == '*') {
    xoaMotKyTu();
  } else if (phimDuocNhan == '#') {
    tinhBieuThuc();
  } else if (phimDuocNhan >= '0' && phimDuocNhan <= '9') {
    nhapChuSo(phimDuocNhan);
  } else {
    char toanTu = doiPhimSangToanTu(phimDuocNhan);
    if (toanTu != '\0') nhapToanTu(toanTu);
  }
  datConTroLenLCD();
}
