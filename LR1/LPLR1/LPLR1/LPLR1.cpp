#include <iostream>
#include <fstream>
#include <cmath>
#include <map>
#include <stdexcept>
using namespace std;

// Клас для винятку при відсутності файлу
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

// Клас для винятку при діленні на нуль
class ErrorDivZero : public exception {
public:
    const char* what() const noexcept override {
        return "Division by zero occurred.";
    }
};

// Функція лінійної інтерполяції значень з файлу
// Приймає ім'я файлу та значення x, повертає відповідне y
// Формат файлу: x <tab> y

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

// Визначає, який файл відкрити для T(x) або U(x) залежно від x
string getTableFile(double x, bool isU) {
    string base = isU ? "dat_X" : "dat_X";
    if (x <= 1 && x >= -1) return "dat_X_1_1.dat";
    if (x < -1) return "dat_X1_00.dat";
    return "dat_X00_1.dat";
}

// Функції T(x) та U(x), що викликають інтерполяцію
// T і U беруться з відповідних таблиць

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

// Функція Qrz: ділить більше число на менше, з перевіркою на 0
double Qrz(double x, double y) {
    if (y == 0 || x == 0) throw ErrorDivZero();
    return (x > y) ? x / y : y / x;
}

// Функція Srs: базується на умовах для обчислення
// або множення й синус, або логарифм виразу

double Srs(double x, double y, double z) {
    if (x + y + z > 0)
        return x * y + sin(z);
    else
        return log(1 + x * x + y * y + z * z);
}

// Прототип Rrz, бо використовується у Krn
double Rrz(double x, double y, double z);

// Функція Krn використовує Rrz двічі з коефіцієнтами

double Krn(double x, double y, double z) {
    return 73.1389 * Rrz(x, y, z) + 14.838 * Rrz(y, z, x);
}

// Функція Rrz: обирає варіант обчислення залежно від суми xyz
// та викликає Qrz. Якщо сума <= 0 — рекурсивно коригує значення

double Rrz(double x, double y, double z) {
    double sum = x + y + z;
    if (sum <= 0) return Rrz(x + 1, y + 1, z + 1); // Імітація Алгоритму 2
    double q = (x + y > z) ? z * Qrz(x, y) : x * Qrz(z, y);
    return q;
}

// Головна функція fun(x, y, z), яка викликає вкладені Krn
// та обробляє можливі винятки

double fun(double x, double y, double z) {
    try {
        return x * Krn(x, y, z) + y * Krn(y, z, x) - z * Krn(z, x, y);
    }
    catch (ErrorNoFile& e) {
        cerr << e.what() << endl;
        return 4.3498 * x + 23.23 * y - 2.348 * x * y * z; // Резервна формула
    }
    catch (ErrorDivZero& e) {
        cerr << e.what() << endl;
        return 4.3498 * x + 23.23 * y - 2.348 * x * y * z; // Резервна формула
    }
    catch (...) {
        cerr << "Unknown error occurred." << endl;
        return 4.3498 * x + 23.23 * y - 2.348 * x * y * z; // Резервна формула
    }
}

// Точка входу
int main() {
    double x, y, z;
    cout << "Enter x, y, z: ";
    cin >> x >> y >> z;

    double result = fun(x, y, z);
    cout << "fun(x, y, z) = " << result << endl;
    return 0;
}