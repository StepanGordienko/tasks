import numpy as np
from sympy import symbols, integrate, lambdify
import matplotlib.pyplot as plt


def f(x):
    return -x ** 2 + 5


a, b = 0, 3
n_values = [10, 50, 100, 500]

print("=" * 70)
print(f"ЧИСЛЕННОЕ ИНТЕГРИРОВАНИЕ")
print(f"Функция: y = -x² + 5")
print(f"Отрезок: [{a}, {b}]")
print("=" * 70)

print("\n1 АНАЛИТИЧЕСКИЙ РАСЧЕТ (SymPy):")
print("-" * 70)

x = symbols('x')

antiderivative = integrate(f(x), x)
print(f"Первообразная F(x) = {antiderivative}")

analytical_result = integrate(f(x), (x, a, b))
print(f"∫f(x)dx на [{a}, {b}] = {analytical_result}")
analytical_result = float(analytical_result)
print(f"Точное значение = {analytical_result:.10f}")

print("\n2  МЕТОД ПРЯМОУГОЛЬНИКОВ:")

def rectangles_left(f, a, b, n):
    h = (b - a) / n
    integral = 0

    for i in range(n):
        x_left = a + i * h
        integral += f(x_left) * h

    return integral


def rectangles_right(f, a, b, n):
    h = (b - a) / n
    integral = 0

    for i in range(1, n + 1):
        x_right = a + i * h
        integral += f(x_right) * h

    return integral


print("Левые прямоугольники:")
for n in n_values:
    result = rectangles_left(f, a, b, n)
    error = abs(result - analytical_result)
    print(f"  n={n:3d}: {result:.10f}, погрешность = {error:.2f}")

print("\nПравые прямоугольники:")
for n in n_values:
    result = rectangles_right(f, a, b, n)
    error = abs(result - analytical_result)
    print(f"  n={n:3d}: {result:.10f}, погрешность = {error:.2f}")

print("\n  МЕТОД ТРАПЕЦИЙ:")

def trapezoids(f, a, b, n):
    h = (b - a) / n
    integral = 0

    integral += f(a) * h / 2

    for i in range(1, n):
        x_i = a + i * h
        integral += f(x_i) * h

    integral += f(b) * h / 2

    return integral


print("Метод трапеций:")
for n in n_values:
    result = trapezoids(f, a, b, n)
    error = abs(result - analytical_result)
    print(f"  n={n:3d}: {result:.10f}, погрешность = {error:.2f}")

print("\n4  МЕТОД ПАРАБОЛ (СИМПСОНА):")

def simpsons_rule(f, a, b, n):
    if n % 2 != 0:
        n += 1

    h = (b - a) / n
    integral = 0

    integral += f(a)

    for i in range(1, n):
        x_i = a + i * h
        coeff = 4 if i % 2 == 1 else 2
        integral += coeff * f(x_i)

    integral += f(b)

    integral *= h / 3

    return integral


print("Метод парабол (Симпсона):")
for n in n_values:
    result = simpsons_rule(f, a, b, n)
    error = abs(result - analytical_result)
    print(f"  n={n:3d}: {result:.10f}, погрешность = {error:.2f}")

print("\n5  СРАВНЕНИЕ ВСЕХ МЕТОДОВ:")
print("-" * 70)

n_test = 100
print(f"\nТочка сравнения: n = {n_test}\n")

methods = {
    "Аналитический": analytical_result,
    "Левые прямоугольники": rectangles_left(f, a, b, n_test),
    "Правые прямоугольники": rectangles_right(f, a, b, n_test),
    "Трапеции": trapezoids(f, a, b, n_test),
    "Симпсон": simpsons_rule(f, a, b, n_test),
}

print(f"{'Метод':<25} {'Результат':<20} {'Ошибка':<15}")

for method_name, result in methods.items():
    if method_name == "Аналитический":
        print(f"{method_name:<25} {result:<20.10f} {'точное значение':<15}")
    else:
        error = abs(result - analytical_result)
        error_percent = (error / analytical_result) * 100 if analytical_result != 0 else 0
        print(f"{method_name:<25} {result:<20.10f} {error:.2f} ({error_percent:.4f}%)")

print("\nАНАЛИЗ СХОДИМОСТИ (как n влияет на точность):")
print(f"{'n':<6} {'Прямоуг. (лев)':<18} {'Трапеции':<18} {'Симпсон':<18}")

for n in [10, 20, 50, 100, 200, 500]:
    err_rect = abs(rectangles_left(f, a, b, n) - analytical_result)
    err_trap = abs(trapezoids(f, a, b, n) - analytical_result)
    err_simp = abs(simpsons_rule(f, a, b, n) - analytical_result)
    print(f"{n:<6} {err_rect:<18.2f} {err_trap:<18.2f} {err_simp:<18.2f}")

print("\nВывод: увеличение n снижает погрешность. Симпсон сходится быстрее всех")