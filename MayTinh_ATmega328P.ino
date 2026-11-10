#include <Arduino.h>
#include <Keypad.h>
#include <LiquidCrystal.h>
#include <stdint.h>
#include <stdio.h>

const byte SO_HANG_PHIM = 4;
const byte SO_COT_PHIM = 4;
const byte SO_COT_LCD = 20;
const byte SO_HANG_LCD = 4;
const byte SO_CHU_SO_TOI_DA = 4;

// 4 chu so + 1 toan tu + 4 chu so + 1 ky tu '\0' = 10 byte.
const byte KICH_THUOC_BO_DEM = 2 * SO_CHU_SO_TOI_DA + 2;

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
LiquidCrystal manHinhLCD(A0, A1, A2, A3, A4, A5);

char boDemBieuThuc[KICH_THUOC_BO_DEM];
char *conTroGhiKyTu = boDemBieuThuc;  // O nho se ghi tiep theo
char *conTroToanTu = nullptr;         // Dia chi toan tu; chua co = nullptr
bool daTinhXong = false;

void xoaDongLCD(byte hang) {
  manHinhLCD.setCursor(0, hang);
  for (byte cot = 0; cot < SO_COT_LCD; ++cot) {
    manHinhLCD.write(' ');
  }
  manHinhLCD.setCursor(0, hang);
}

// In chuoi hang duoc luu o Flash, truyen vao bang F("...").
void inDongFlash(byte hang, const __FlashStringHelper *noiDung) {
  xoaDongLCD(hang);
  manHinhLCD.print(noiDung);
}

// In chuoi nam trong SRAM, doc chuoi thong qua con tro.
void inDongSRAM(byte hang, const char *conTroChuoi) {
  xoaDongLCD(hang);
  manHinhLCD.print(conTroChuoi);
}

void hienThiBieuThuc() {
  xoaDongLCD(1);
  manHinhLCD.print(F("BT: "));
  manHinhLCD.print(boDemBieuThuc);
}

void xoaBieuThuc() {
  conTroGhiKyTu = boDemBieuThuc;
  conTroToanTu = nullptr;
  *conTroGhiKyTu = '\0';
  daTinhXong = false;

  hienThiBieuThuc();
  inDongFlash(2, F("NHAP BIEU THUC"));
}

bool ghiKyTuVaoBoDem(char kyTu) {
  if (conTroGhiKyTu >= boDemBieuThuc + KICH_THUOC_BO_DEM - 1) {
    inDongFlash(2, F("LOI: BO DEM DAY"));
    return false;
  }

  *conTroGhiKyTu = kyTu;  // Ghi qua con tro (truy cap gian tiep)
  ++conTroGhiKyTu;       // Chuyen sang dia chi o nho ke tiep
  *conTroGhiKyTu = '\0'; // Ket thuc chuoi
  hienThiBieuThuc();
  return true;
}

void nhapChuSo(char chuSo) {
  if (daTinhXong) {
    xoaBieuThuc(); // Nhan so moi sau ket qua -> phep tinh moi
  }

  byte soChuSoDaNhap;
  if (conTroToanTu == nullptr) {
    soChuSoDaNhap = (byte)(conTroGhiKyTu - boDemBieuThuc);
  } else {
    soChuSoDaNhap = (byte)(conTroGhiKyTu - (conTroToanTu + 1));
  }

  if (soChuSoDaNhap >= SO_CHU_SO_TOI_DA) {
    inDongFlash(2, F("TOI DA 4 CHU SO"));
    return;
  }

  if (ghiKyTuVaoBoDem(chuSo)) {
    inDongFlash(2, F("NHAN # DE TINH"));
  }
}

void nhapToanTu(char toanTu) {
  if (daTinhXong) {
    xoaBieuThuc();
  }

  if (conTroGhiKyTu == boDemBieuThuc) {
    inDongFlash(2, F("NHAP SO THU NHAT"));
    return;
  }

  if (conTroToanTu != nullptr) {
    inDongFlash(2, F("CHI DUOC 1 TOAN TU"));
    return;
  }

  if (ghiKyTuVaoBoDem(toanTu)) {
    conTroToanTu = conTroGhiKyTu - 1;
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

  *conTroGiaTriSo = soDangDoc; // Ghi gia tri vao bien ben ngoai ham
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
  if (toanTu != '+' && toanTu != '-' && toanTu != '*' && toanTu != '/') {
    inDongFlash(2, F("TOAN TU KHONG HOP LE"));
    return;
  }

  const char *conTroBatDauSoHai = conTroViTriToanTu + 1;
  const char *conTroKetThuc = docSoNguyen(conTroBatDauSoHai, &soThuHai);
  if (conTroKetThuc == conTroBatDauSoHai || *conTroKetThuc != '\0') {
    inDongFlash(2, F("SAI BIEU THUC"));
    return;
  }

  char chuoiKetQua[SO_COT_LCD + 1]; // 21 byte tam tren stack
  int32_t ketQua = 0;

  switch (toanTu) {
    case '+':
      ketQua = soThuNhat + soThuHai;
      break;
    case '-':
      ketQua = soThuNhat - soThuHai;
      break;
    case '*':
      ketQua = soThuNhat * soThuHai;
      break;
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

// Ban phim A,B,C,D chuyen thanh ky tu toan hoc +,-,*,/.
char doiPhimSangToanTu(char phim) {
  switch (phim) {
    case 'A': return '+';
    case 'B': return '-';
    case 'C': return '*';
    case 'D': return '/';
    default:  return '\0';
  }
}

void setup() {
  banPhim.setDebounceTime(20);
  manHinhLCD.begin(SO_COT_LCD, SO_HANG_LCD);
  inDongFlash(0, F("A:+ B:- C:* D:/"));
  inDongFlash(3, F("*=XOA      #=TINH"));
  xoaBieuThuc();
}

void loop() {
  char phimDuocNhan = banPhim.getKey();
  if (phimDuocNhan == NO_KEY) {
    return;
  }

  if (phimDuocNhan == '*') {
    xoaBieuThuc();
  } else if (phimDuocNhan == '#') {
    tinhBieuThuc();
  } else if (phimDuocNhan >= '0' && phimDuocNhan <= '9') {
    nhapChuSo(phimDuocNhan);
  } else {
    char toanTu = doiPhimSangToanTu(phimDuocNhan);
    if (toanTu != '\0') {
      nhapToanTu(toanTu);
    }
  }
}
