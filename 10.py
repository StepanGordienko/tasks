import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

plt.style.use('seaborn-v0_8-darkgrid')
plt.rcParams['figure.figsize'] = (12, 7)

df = pd.read_csv('data.csv', sep=';')

x = df['x'].values
y = df['y'].values

plt.figure(figsize=(12, 6))
plt.plot(x, y, 'b-', linewidth=1, alpha=0.7, label='Исходные данные')
plt.title('Исходный временной ряд')
plt.xlabel('x')
plt.ylabel('y')
plt.legend()
plt.grid(True, alpha=0.3)
plt.tight_layout()
plt.show()

N = len(y)
print(f"Длина наблюдений N = {N}")


def moving_average(y, L):
    N = len(y)
    z = np.full(N, np.nan)
    for k in range(L, N - L):
        z[k] = np.mean(y[k - L: k + L + 1])

    return z

L_values = [1, 3, 5, 10]

plt.figure(figsize=(14, 10))
plt.plot(x, y, 'b-', linewidth=1, alpha=0.5, label='Исходные данные')

colors = ['red', 'green', 'orange', 'purple']
for L, color in zip(L_values, colors):
    z = moving_average(y, L)
    plt.plot(x[L: N - L], z[L: N - L], '-', color=color, linewidth=2,
             label=f'Скользящее среднее, L={L}')

plt.title('Сглаживание методом скользящего среднего')
plt.xlabel('x')
plt.ylabel('y')
plt.legend()
plt.grid(True, alpha=0.3)
plt.tight_layout()
plt.show()


def exponential_smoothing(y, alpha):
    N = len(y)
    t = np.zeros(N)
    t[0] = y[0]
    for k in range(1, N):
        t[k] = alpha * y[k] + (1 - alpha) * t[k - 1]

    return t

alpha_values = [0.1, 0.3, 0.5, 0.7, 0.9]

plt.figure(figsize=(14, 10))
plt.plot(x, y, 'b-', linewidth=1, alpha=0.5, label='Исходные данные')

colors = ['red', 'green', 'orange', 'purple', 'brown']
for alpha, color in zip(alpha_values, colors):
    t = exponential_smoothing(y, alpha)
    plt.plot(x, t, '-', color=color, linewidth=2,
             label=f'Экспоненциальное сглаживание, α={alpha}, β={1 - alpha:.1f}')

plt.title('Экспоненциальное сглаживание временного ряда')
plt.xlabel('x')
plt.ylabel('y')
plt.legend()
plt.grid(True, alpha=0.3)
plt.tight_layout()
plt.show()

print("\n=== Анализ результатов ===")
print(f"Исходные данные: min(y) = {np.min(y):.4f}, max(y) = {np.max(y):.4f}, "
      f"среднее = {np.mean(y):.4f}, std = {np.std(y):.4f}")


z_3 = moving_average(y, 3)
valid_mask = ~np.isnan(z_3)
print(f"\nСкользящее среднее (L=3):")
print(f"  min = {np.min(z_3[valid_mask]):.4f}, max = {np.max(z_3[valid_mask]):.4f}, "
      f"среднее = {np.mean(z_3[valid_mask]):.4f}")


t_03 = exponential_smoothing(y, 0.3)
print(f"\nЭкспоненциальное сглаживание (α=0.3, β=0.7):")
print(f"  min = {np.min(t_03):.4f}, max = {np.max(t_03):.4f}, "
      f"среднее = {np.mean(t_03):.4f}")

plt.figure(figsize=(14, 7))
plt.plot(x, y, 'b-', linewidth=1, alpha=0.5, label='Исходные данные')
plt.plot(x[3:N - 3], z_3[3:N - 3], 'r-', linewidth=2, label='Скользящее среднее (L=3)')
plt.plot(x, t_03, 'g-', linewidth=2, label='Экспоненциальное сглаживание (α=0.3, β=0.7)')

plt.title('Сравнение методов сглаживания (оптимальные параметры)')
plt.xlabel('x')
plt.ylabel('y')
plt.legend()
plt.grid(True, alpha=0.3)
plt.tight_layout()
plt.show()

print("\nНаблюдения:")
print("- Метод скользящего среднего: сглаживает резкие выбросы, но теряет данные на краях")
print("- Экспоненциальное сглаживание: сохраняет все точки, более плавно реагирует на изменения")
print("- Меньшие значения α дают более сильное сглаживание, большие α - ближе к исходным данным")