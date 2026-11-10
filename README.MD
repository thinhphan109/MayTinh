# MÁY TÍNH 4 PHÉP TÍNH – BỘ NHỚ VÀ CON TRỎ

**Nền tảng:** ATmega328P (Arduino Uno R3) · **Hiển thị:** LCD 20×4, giao tiếp song song 4-bit · **Nhập liệu:** Keypad ma trận 4×4

## 1. Thông tin đề tài và sơ đồ kết nối

Khối mô tả dự án dưới đây được giữ nguyên theo yêu cầu và cũng có ở đầu file `.ino`:

```cpp
/*
  DE TAI: MAY TINH 4 PHEP TINH - BO NHO VA CON TRO
  Vi dieu khien: ATmega328P (Arduino Uno R3)
  Man hinh: LCD 20x4, giao tiep song song 4-bit
  Ban phim: Keypad 4x4

  Ket noi Keypad:
    Hang R1,R2,R3,R4 -> D8,D9,D10,D11
    Cot  C1,C2,C3,C4 -> D2,D3,D4,D5

  Ket noi LCD:
    RS -> A0; E -> A1
    D4 -> A2; D5 -> A3; D6 -> A4; D7 -> A5
    RW -> GND; VSS -> GND; VDD -> 5V

  Phim A = +; B = -; C = *; D = /
  Phim * = xoa; phim # = tinh
  Chi tinh 2 so, moi so toi da 4 chu so (0..9999).
*/
```

## 2. Chức năng

Chương trình nhận **hai số nguyên không âm**, mỗi số có **1–4 chữ số** (giá trị từ 0 đến 9999), và **một toán tử**. Sau khi nhấn `#`, chương trình phân tích biểu thức rồi hiển thị kết quả.

- `A` → cộng (`+`); `B` → trừ (`-`); `C` → nhân (`*`); `D` → chia (`/`).
- `*` → xóa biểu thức hiện tại; `#` → tính kết quả.
- Phép chia là **chia nguyên**, hiển thị thương và số dư; báo lỗi nếu chia cho 0.
- Phép trừ có thể cho kết quả âm. Chưa hỗ trợ nhập số âm trực tiếp, số thập phân, chuỗi nhiều phép toán hoặc ưu tiên phép toán.
- Sau khi có kết quả, nhấn **chữ số** hoặc **toán tử** sẽ bắt đầu một biểu thức mới. Nhấn `#` lần nữa thì tính lại biểu thức đang có.

**Ví dụ kiểm tra:**

| Phím cần nhấn | Biểu thức | Kết quả dự kiến |
|---|---|---|
| `9 A 9 #` | `9+9` | `KQ: 18` |
| `999 A 9 #` | `999+9` | `KQ: 1008` |
| `9999 A 9999 #` | `9999+9999` | `KQ: 19998` |
| `100 B 250 #` | `100-250` | `KQ: -150` |
| `9999 C 9999 #` | `9999*9999` | `KQ: 99980001` |
| `7 D 3 #` | `7/3` | `KQ: 2 DU: 1` |
| `7 D 0 #` | `7/0` | `LOI: CHIA CHO 0` |

**Giới hạn:** `12345+1` không nhập được trọn vẹn vì số đầu có 5 chữ số. `9+9+9` không được hỗ trợ vì chương trình chỉ cho một toán tử. Với hai số nhập tối đa 9999, kết quả nhân lớn nhất là **99.980.001**, nằm trong miền số nguyên có dấu 32-bit.

## 3. Linh kiện và đấu nối

- 01 Arduino Uno R3 (ATmega328P).
- 01 LCD ký tự 20×4 **chuẩn giao tiếp song song HD44780**, không phải loại chỉ có bốn chân I²C.
- 01 bàn phím ma trận 4×4.
- 01 biến trở chỉnh độ tương phản LCD (thường 10 kΩ), dây nối, nguồn USB/5 V phù hợp.

### Bàn phím 4×4

| Tín hiệu keypad | Arduino Uno R3 |
|---|---|
| R1, R2, R3, R4 | D8, D9, D10, D11 |
| C1, C2, C3, C4 | D2, D3, D4, D5 |

> Thứ tự chân cáp của keypad có thể khác nhau giữa các mẫu. Cần xác nhận R1–R4 và C1–C4 của bàn phím đang sử dụng; không mặc định thứ tự 8 chân vật lý từ trái sang phải.

### LCD 20×4 ở chế độ 4-bit

| Chân LCD | Kết nối |
|---|---|
| VSS | GND |
| VDD | 5V |
| V0 (độ tương phản) | Chân giữa biến trở; hai đầu biến trở nối 5V và GND |
| RS | A0 |
| RW | GND |
| E | A1 |
| D4 | A2 |
| D5 | A3 |
| D6 | A4 |
| D7 | A5 |
| D0–D3 | Không sử dụng trong chế độ 4-bit |
| A/K (nếu có chân đèn nền) | Đấu theo thông số LCD/module; dùng hạn dòng khi cần |

**Lưu ý:** A0–A5 đang dùng như chân digital. A4, A5 ở cấu hình này đã dành cho LCD, không đồng thời dùng làm SDA/SCL của I²C. Các thiết bị phải **chung GND**.

## 4. Cài đặt và nạp chương trình

1. Cài **Arduino IDE**, chọn **Tools → Board → Arduino AVR Boards → Arduino Uno** và đúng cổng COM.
2. Cài thư viện **Keypad** qua Library Manager (thư viện Keypad phổ biến của Mark Stanley, Alexander Brevig). Thư viện **LiquidCrystal** thường có sẵn cùng Arduino IDE, kiểm tra nếu IDE báo thiếu.
3. Mở file `MayTinh_ATmega328P_LCD20x4_BienDeHieu.ino` trong thư mục có cùng tên.
4. Kiểm tra kết nối, chọn **Verify/Compile** rồi **Upload** lên Arduino Uno R3.
5. Cấp nguồn, chỉnh biến trở V0 để thấy rõ ký tự, kiểm tra lần lượt `9 A 9 #`, `999 A 9 #` và các trường hợp trong bảng trên.

Nếu chỉ thấy ô vuông hoặc không thấy chữ, kiểm tra nguồn, GND, chân RS/E/D4–D7 và điều chỉnh V0. Nếu phím nhập sai ký tự, kiểm tra thứ tự hàng/cột của keypad.

## 5. Bố cục hiển thị LCD

- **Hàng 1 (chỉ số 0):** `A:+ B:- C:* D:/` – hướng dẫn toán tử.
- **Hàng 2 (chỉ số 1):** `BT: 999+9` – biểu thức đang nhập.
- **Hàng 3 (chỉ số 2):** `KQ: 1008`, thông báo hướng dẫn hoặc lỗi.
- **Hàng 4 (chỉ số 3):** `*=XOA      #=TINH` – hướng dẫn hai phím chức năng.

## 6. Cấu trúc xử lý chương trình

```text
Khởi động ATmega328P
  └─ setup()
      ├─ Cấu hình chống dội Keypad
      ├─ Khởi tạo LCD 20×4
      └─ xoaBieuThuc()  [đặt lại bộ đệm và con trỏ]

loop() – lặp liên tục
  └─ Đọc banPhim.getKey()
      ├─ '*' → xoaBieuThuc()
      ├─ '#' → tinhBieuThuc()
      │          ├─ docSoNguyen() → số thứ nhất
      │          ├─ Đọc toán tử
      │          ├─ docSoNguyen() → số thứ hai
      │          └─ Thực hiện phép tính, đưa kết quả ra LCD
      ├─ '0'..'9' → nhapChuSo() → ghiKyTuVaoBoDem()
      └─ 'A'..'D' → doiPhimSangToanTu() → nhapToanTu()
```

## 7. Kỹ thuật bộ nhớ và con trỏ – trọng tâm nghiên cứu

### 7.1. Bộ đệm tĩnh trong SRAM

```cpp
const byte SO_CHU_SO_TOI_DA = 4;
const byte KICH_THUOC_BO_DEM = 2 * SO_CHU_SO_TOI_DA + 2;
char boDemBieuThuc[KICH_THUOC_BO_DEM];
```

Bộ đệm có **10 byte**: 4 ký tự số + 1 toán tử + 4 ký tự số + 1 ký tự `\0` kết thúc chuỗi. Đây là một mảng toàn cục có kích thước cố định, không cấp phát động trong lúc nhập liệu.

Ví dụ `999+9` được lưu như sau (ô sau `\0` không thuộc chuỗi hiện tại):

```text
Chỉ số:      0    1    2    3    4     5    6    7    8    9
Dữ liệu:    '9'  '9'  '9'  '+'  '9'  '\0'   -    -    -    -
                                         ^
                                   conTroGhiKyTu
```

### 7.2. Con trỏ ghi dữ liệu

```cpp
char *conTroGhiKyTu = boDemBieuThuc;

*conTroGhiKyTu = kyTu;   // Ghi ký tự vào ô nhớ đang được trỏ tới
++conTroGhiKyTu;        // Trỏ sang ô tiếp theo
*conTroGhiKyTu = '\0'; // Đánh dấu kết thúc chuỗi
```

- `conTroGhiKyTu`: biến chứa địa chỉ cần ghi.
- `*conTroGhiKyTu`: truy cập dữ liệu **tại địa chỉ** đó (giải tham chiếu).
- `++conTroGhiKyTu`: dịch sang phần tử tiếp theo của mảng `char`.
- `\0`: ký tự kết thúc chuỗi C; khi xóa biểu thức chỉ cần ghi `\0` ở ô đầu để biến chuỗi thành rỗng về mặt logic.

### 7.3. Con trỏ toán tử và phép trừ con trỏ

```cpp
char *conTroToanTu = nullptr;

// Khi vừa ghi xong toán tử:
conTroToanTu = conTroGhiKyTu - 1;

// Đếm chữ số của số thứ hai:
soChuSoDaNhap = (byte)(conTroGhiKyTu - (conTroToanTu + 1));
```

`conTroToanTu` ghi nhớ **địa chỉ ký tự toán tử** trong cùng mảng. Phép trừ hai con trỏ thuộc cùng mảng trả về **số phần tử** ở giữa, dùng để kiểm tra giới hạn 4 chữ số cho từng toán hạng.

### 7.4. Đọc số qua con trỏ và truyền địa chỉ vào hàm

```cpp
const char *docSoNguyen(const char *conTroDocKyTu, int32_t *conTroGiaTriSo);

// Cách gọi:
const char *conTroViTriToanTu = docSoNguyen(boDemBieuThuc, &soThuNhat);
```

- `conTroDocKyTu` đi qua các ký tự số; `const char *` ngăn sửa ký tự qua con trỏ đọc.
- Hàm đổi `'9'` thành 9 bằng phép toán `*conTroDocKyTu - '0'`.
- `&soThuNhat` đưa **địa chỉ** của biến vào hàm; `*conTroGiaTriSo = soDangDoc` ghi kết quả vào chính biến đó.
- Giá trị trả về của hàm là con trỏ trỏ tới ký tự **chưa được đọc** đầu tiên (ví dụ dấu `+`).

### 7.5. Bảo vệ giới hạn bộ đệm

```cpp
if (conTroGhiKyTu >= boDemBieuThuc + KICH_THUOC_BO_DEM - 1) {
  inDongFlash(2, F("LOI: BO DEM DAY"));
  return false;
}
```

Kiểm tra này ngăn ghi tràn mảng và dành byte cuối cho `\0`. Ngoài ra `nhapChuSo()` còn kiểm tra tối đa 4 chữ số **riêng cho mỗi toán hạng**.

### 7.6. Flash so với SRAM

- `F("...")` được dùng cho các thông báo LCD cố định để đọc trực tiếp từ vùng chương trình **Flash**, hạn chế sử dụng SRAM.
- `boDemBieuThuc` là dữ liệu thay đổi khi nhập phím, vì vậy nằm ở **SRAM**.
- `chuoiKetQua[21]` là bộ đệm cục bộ để định dạng chuỗi kết quả rồi hiển thị.
- `int32_t` giúp giữ kết quả phép nhân lớn nhất **99.980.001**, không bị giới hạn bởi `int` 16-bit trên AVR.
- Chương trình **không sử dụng EEPROM**, và **không thực hiện ánh xạ địa chỉ thanh ghi I/O bằng con trỏ**; đây là đề tài thao tác dữ liệu trong bộ nhớ bằng con trỏ, không phải đề tài lập trình thanh ghi trực tiếp.

## 8. Tên các biến và hàm chính

| Tên trong code | Ý nghĩa |
|---|---|
| `boDemBieuThuc` | Mảng ký tự lưu biểu thức |
| `conTroGhiKyTu` | Con trỏ chỉ ô sẽ ghi tiếp theo |
| `conTroToanTu` | Con trỏ trỏ đến toán tử đã nhập |
| `conTroDocKyTu` | Con trỏ đọc từng ký tự |
| `soThuNhat`, `soThuHai` | Hai toán hạng số nguyên |
| `ketQua` | Kết quả cộng/trừ/nhân |
| `chuoiKetQua` | Chuỗi để hiển thị kết quả lên LCD |
| `daTinhXong` | Cờ logic đánh dấu đã tính xong |
| `ghiKyTuVaoBoDem()` | Ghi ký tự vào SRAM qua con trỏ |
| `nhapChuSo()` | Nhận chữ số, kiểm tra giới hạn |
| `nhapToanTu()` | Nhận một toán tử hợp lệ |
| `docSoNguyen()` | Đọc chuỗi số thành số nguyên, qua con trỏ |
| `tinhBieuThuc()` | Kiểm tra biểu thức, tính và in kết quả |
| `xoaBieuThuc()` | Xóa chuỗi logic, đặt lại con trỏ và trạng thái |

## 9. Gợi ý nội dung thực nghiệm để đưa vào báo cáo

1. Nhập và tính các trường hợp hợp lệ: số 1 chữ số, 4 chữ số, phép cộng/trừ/nhân/chia, kết quả âm và chia có dư.
2. Thử nhập **5 chữ số** cho một toán hạng; ghi nhận LCD báo `TOI DA 4 CHU SO` và không ghi thêm ký tự.
3. Thử nhập toán tử trước số đầu, nhập hai toán tử, bấm `#` khi thiếu số thứ hai, chia cho 0.
4. Theo dõi mô phỏng: sau `999+9`, con trỏ ghi trỏ tới phần tử 5, con trỏ toán tử trỏ tới phần tử 3.
5. Dùng kết quả biên dịch Arduino IDE để ghi dung lượng **Flash** và **SRAM** thực tế; chỉ đưa số liệu sau khi tự biên dịch, không suy ra tổng bộ nhớ từ bộ đệm 10 byte.

**File mã nguồn:** `MayTinh_ATmega328P_LCD20x4_BienDeHieu.ino`  
**Chủ đề cốt lõi:** quản lý bộ đệm SRAM, con trỏ ghi/đọc, phép toán con trỏ, truyền địa chỉ cho hàm và lưu chuỗi hằng trong Flash.
