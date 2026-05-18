import numpy as np
import matplotlib.pyplot as plt


data = np.loadtxt('lab2_data.csv', delimiter=';', skiprows=1, dtype=float)
x = data[:, 0]
y = data[:, 1]

plt.figure(figsize=(10, 5))
plt.plot(x, y, marker='o', linewidth=0.5, linestyle='--', color='red', markerfacecolor='blue', label='Исходные данные')
plt.title('Исходная таблично заданная функция')
plt.legend()
plt.show()


i_tmp = 7
A_lin_9 = [[x[i_tmp], 1], [x[i_tmp+2], 1]]
B_lin_9 = [y[i_tmp], y[i_tmp+2]]
A_inv_lin_9 = np.linalg.inv(A_lin_9)
X_lin_9 = np.dot(A_inv_lin_9, B_lin_9)
a_lin_9, b_lin_9 = X_lin_9[0], X_lin_9[1]
y_lin_9 = a_lin_9 * 9 + b_lin_9



i1, i2, i3 = 6, 7, 9
A_quad_9 = [[x[i1]**2, x[i1], 1], [x[i2]**2, x[i2], 1], [x[i3]**2, x[i3], 1]]
B_quad_9 = [y[i1], y[i2], y[i3]]
A_inv_quad_9 = np.linalg.inv(A_quad_9)
X_quad_9 = np.dot(A_inv_quad_9, B_quad_9)
a_q9, b_q9, c_q9 = X_quad_9[0], X_quad_9[1], X_quad_9[2]
y_quad_9 = a_q9 * 9**2 + b_q9 * 9 + c_q9


i_tmp = 37
A_lin_39 = [[x[i_tmp], 1],[x[i_tmp+2], 1]]
B_lin_39 = [y[i_tmp], y[i_tmp+2]]
A_inv_lin_39 = np.linalg.inv(A_lin_39)
X_lin_39 = np.dot(A_inv_lin_39, B_lin_39)
a_lin_39, b_lin_39 = X_lin_39[0], X_lin_39[1]
y_lin_39 = a_lin_39 * 39 + b_lin_39



i1, i2, i3 = 36, 37, 39
A_quad_39 = [[x[i1]**2, x[i1], 1],
             [x[i2]**2, x[i2], 1],
             [x[i3]**2, x[i3], 1]]
B_quad_39 = [y[i1], y[i2], y[i3]]
A_inv_quad_39 = np.linalg.inv(A_quad_39)
X_quad_39 = np.dot(A_inv_quad_39, B_quad_39)
a_q39, b_q39, c_q39 = X_quad_39[0], X_quad_39[1], X_quad_39[2]
y_quad_39 = a_q39 * 39**2 + b_q39 * 39 + c_q39

print(f"x = 9:")
print(f"  Линейная: y = {y_lin_9:.3f}")
print(f"  Квадратичная: y = {y_quad_9:.3f}")
print(f"\nx = 39:")
print(f"  Линейная: y = {y_lin_39:.3f}")
print(f"  Квадратичная: y = {y_quad_39:.3f}")

y_lin_filled = y.copy()
y_quad_filled = y.copy()
y_lin_filled[8] = y_lin_9
y_lin_filled[38] = y_lin_39
y_quad_filled[8] = y_quad_9
y_quad_filled[38] = y_quad_39


plt.figure(figsize=(12, 6))

plt.plot(x, y, marker = 'o', linewidth = 0, markerfacecolor='blue', label = 'исходные данные')

x_lin_seg_9 = np.array([8, 10])
y_lin_seg_9 = np.array([y[7], y[9]])
plt.plot(x_lin_seg_9, y_lin_seg_9, 's-', color='orange', linewidth=1.5,label='Линейная интерполяция (отрезки)')

x_lin_seg_39 = np.array([38, 40])
y_lin_seg_39 = np.array([y[37], y[39]])
plt.plot(x_lin_seg_39, y_lin_seg_39, 's-', color='orange', linewidth=1.5)


x_quad_9 = np.linspace(7, 10, 50)
y_quad_curve_9 = a_q9 * x_quad_9**2 + b_q9 * x_quad_9 + c_q9
plt.plot(x_quad_9, y_quad_curve_9, 'd-', color='green', linewidth=1.5,label='Квадратичная интерполяция (параболы)')

x_quad_39 = np.linspace(37, 40, 50)
y_quad_curve_39 = a_q39 * x_quad_39**2 + b_q39 * x_quad_39 + c_q39
plt.plot(x_quad_39, y_quad_curve_39, 'd-', color='green', linewidth=1.5)


plt.plot(9, y_lin_9, 'o', color='red', markersize=8,label=f'Лин. x=9 ({y_lin_9:.1f})')
plt.plot(9, y_quad_9, 'o', color='darkgreen', markersize=8,label=f'Квад. x=9 ({y_quad_9:.1f})')
plt.plot(39, y_lin_39, '^', color='red', markersize=8,label=f'Лин. x=39 ({y_lin_39:.1f})')
plt.plot(39, y_quad_39, '^', color='darkgreen', markersize=8,label=f'Квад. x=39 ({y_quad_39:.1f})')

plt.title('Заполнение пропусков методами линейной и квадратичной интерполяции')
plt.legend()
plt.show()

fig, axes = plt.subplots(1, 2, figsize=(14, 5))

ax = axes[0]
ax.plot(x, y, 'o--', color='red', markerfacecolor='blue', label='Исходные')
ax.plot([8, 10], [y[7], y[9]], 's-', color='orange', label='Линейная')
ax.plot(x_quad_9, y_quad_curve_9, 'd-', color='green', label='Квадратичная')
ax.plot(9, y_lin_9, 'ro', markersize=8)
ax.plot(9, y_quad_9, 'go', markersize=8)
ax.set_xlim(6.5, 10.5)
ax.set_ylim(min(y[6:10])-10, max(y[6:10])+20)
ax.set_title('Интерполяция в точке x=9')
ax.legend()
ax.grid(True)


ax = axes[1]
ax.plot(x, y, 'o--', color='red', markerfacecolor='blue', label='Исходные')
ax.plot([38, 40], [y[37], y[39]], 's-', color='orange', label='Линейная')
ax.plot(x_quad_39, y_quad_curve_39, 'd-', color='green', label='Квадратичная')
ax.plot(39, y_lin_39, 'r^', markersize=8)
ax.plot(39, y_quad_39, 'g^', markersize=8)
ax.set_xlim(36.5, 40.5)
ax.set_ylim(min(y[36:40])-10, max(y[36:40])+20)
ax.set_title('Интерполяция в точке x=39')
ax.legend()
ax.grid(True)

plt.tight_layout()
plt.show()