#include <iostream>
#include <fstream>
#include <cmath>
#include <map>
#include <stdexcept>
using namespace std;

// ���� ��� ������� ��� ��������� �����
class ErrorNoFile : public exception {
    string msg;
public:
    ErrorNoFile(const string& filename) {
        msg = "Cannot open file: " + filename;
    }
    const char* what() const noexcept override {
        return msg.c_str();
    }
};

// ���� ��� ������� ��� ����� �� ����
class ErrorDivZero : public exception {
public:
    const char* what() const noexcept override {
        return "Division by zero occurred.";
    }
};

// ������� ����� ������������ ������� � �����
// ������ ��'� ����� �� �������� x, ������� �������� y
// ������ �����: x <tab> y

// Linear interpolation for T(x) and U(x)
double interpolate(const string& filename, double x) {
    ifstream file(filename);
    if (!file.is_open()) throw ErrorNoFile(filename);

    double x1, y1, x2, y2;
    file >> x1 >> y1;
    if (x == x1) return y1;

    while (file >> x2 >> y2) {
        if (x == x2) return y2;
        if (x > x1 && x < x2) {
            return y1 + (y2 - y1) * (x - x1) / (x2 - x1);
        }
        x1 = x2;
        y1 = y2;
    }
    throw out_of_range("x is out of interpolation range");
}

// �������, ���� ���� ������� ��� T(x) ��� U(x) ������� �� x
string getTableFile(double x, bool isU) {
    string base = isU ? "dat_X" : "dat_X";
    if (x <= 1 && x >= -1) return "dat_X_1_1.dat";
    if (x < -1) return "dat_X1_00.dat";
    return "dat_X00_1.dat";
}

// ������� T(x) �� U(x), �� ���������� ������������
// T � U �������� � ��������� �������

double T(double x) {
    double val = abs(x);
    string file = getTableFile(x, false);
    return interpolate(file, val);
}

double U(double x) {
    double val = abs(x);
    string file = getTableFile(x, true);
    return interpolate(file, val);
}

// ������� Qrz: ����� ����� ����� �� �����, � ��������� �� 0
double Qrz(double x, double y) {
    if (y == 0 || x == 0) throw ErrorDivZero();
    return (x > y) ? x / y : y / x;
}

// ������� Srs: �������� �� ������ ��� ����������
// ��� �������� � �����, ��� �������� ������

double Srs(double x, double y, double z) {
    if (x + y + z > 0)
        return x * y + sin(z);
    else
        return log(1 + x * x + y * y + z * z);
}

// �������� Rrz, �� ��������������� � Krn
double Rrz(double x, double y, double z);

// ������� Krn ����������� Rrz ���� � �������������

double Krn(double x, double y, double z) {
    return 73.1389 * Rrz(x, y, z) + 14.838 * Rrz(y, z, x);
}

// ������� Rrz: ����� ������ ���������� ������� �� ���� xyz
// �� ������� Qrz. ���� ���� <= 0 � ���������� ������ ��������

double Rrz(double x, double y, double z) {
    double sum = x + y + z;
    if (sum <= 0) return Rrz(x + 1, y + 1, z + 1); // ������� ��������� 2
    double q = (x + y > z) ? z * Qrz(x, y) : x * Qrz(z, y);
    return q;
}

// ������� ������� fun(x, y, z), ��� ������� ������� Krn
// �� �������� ������ �������

double fun(double x, double y, double z) {
    try {
        return x * Krn(x, y, z) + y * Krn(y, z, x) - z * Krn(z, x, y);
    }
    catch (ErrorNoFile& e) {
        cerr << e.what() << endl;
        return 4.3498 * x + 23.23 * y - 2.348 * x * y * z; // �������� �������
    }
    catch (ErrorDivZero& e) {
        cerr << e.what() << endl;
        return 4.3498 * x + 23.23 * y - 2.348 * x * y * z; // �������� �������
    }
    catch (...) {
        cerr << "Unknown error occurred." << endl;
        return 4.3498 * x + 23.23 * y - 2.348 * x * y * z; // �������� �������
    }
}

// ����� �����
int main() {
    double x, y, z;
    cout << "Enter x, y, z: ";
    cin >> x >> y >> z;

    double result = fun(x, y, z);
    cout << "fun(x, y, z) = " << result << endl;
    return 0;
}