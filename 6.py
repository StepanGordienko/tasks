import pandas as pd
import matplotlib.pyplot as plt

# Загрузка данных
df = pd.read_csv('company_sales_data.csv')

# Задание 1
plt.figure()
plt.plot(df['month_number'], df['total_profit'], label='Profit per month')
plt.xlabel('Номер месяца')
plt.ylabel('Общая прибыль')
plt.title('Company profit per month')
plt.legend()
plt.grid(True)
plt.show()

# Задание 2
plt.figure()
plt.plot(df['month_number'], df['total_units'], linestyle='--', color='red', marker='o', linewidth=3, label='Total units sold')
plt.xlabel('Номер месяца')
plt.ylabel('Количество проданных единиц')
plt.title('Company Sales data of last year')
plt.legend(loc='lower right')
plt.grid(True)
plt.show()

# Задание 3 (1) - все продукты на одном графике
plt.figure()
plt.plot(df['month_number'], df['facecream'], label='Face Cream')
plt.plot(df['month_number'], df['facewash'], label='Face Wash')
plt.plot(df['month_number'], df['toothpaste'], label='Toothpaste')
plt.plot(df['month_number'], df['bathingsoap'], label='Bathing Soap')
plt.plot(df['month_number'], df['shampoo'], label='Shampoo')
plt.plot(df['month_number'], df['moisturizer'], label='Moisturizer')
plt.xlabel('Month Number')
plt.ylabel('Sales data')
plt.title('Sales data of all products')
plt.legend()
plt.grid(True)
plt.show()

# Задание 3 (2) - каждый продукт на отдельном графике
products = ['facecream', 'facewash', 'toothpaste', 'bathingsoap', 'shampoo', 'moisturizer']
titles = ['Face Cream', 'Face Wash', 'Toothpaste', 'Bathing Soap', 'Shampoo', 'Moisturizer']

for i, prod in enumerate(products):
    plt.figure(figsize=(8, 4))
    plt.plot(df['month_number'], df[prod], marker='o')
    plt.xlabel('Month Number')
    plt.ylabel('Sales data')
    plt.title(f'Sales data of a {titles[i]}')
    plt.grid(True)
    plt.show()

# Задание 4
plt.figure()
plt.scatter(df['month_number'], df['toothpaste'], color='blue', label='Tooth paste Sales data')
plt.xlabel('Month Number')
plt.ylabel('Number of units Sold')
plt.title('Toothpaste Sales per month')
plt.legend()
plt.grid(True, linestyle='--')
plt.show()

# Задание 5
plt.figure()
bar_width = 0.35
x = df['month_number']
plt.bar(x - bar_width/2, df['facecream'], width=bar_width, label='Face Cream')
plt.bar(x + bar_width/2, df['facewash'], width=bar_width, label='Face Wash')
plt.xlabel('Month Number')
plt.ylabel('Sales units in number')
plt.title('Face Cream vs Face Wash Sales')
plt.legend()
plt.grid(True)
plt.show()

# Задание 6
yearly_sales = df[['facecream', 'facewash', 'toothpaste', 'bathingsoap', 'shampoo', 'moisturizer']].sum()
plt.figure()
plt.pie(yearly_sales, labels=yearly_sales.index, autopct='%1.1f%%', startangle=90)
plt.title('Sales data')
plt.show()

# Задание 7
plt.figure()
plt.stackplot(df['month_number'],
              df['facecream'], df['facewash'], df['toothpaste'],
              df['bathingsoap'], df['shampoo'], df['moisturizer'],
              labels=['Face Cream', 'Face Wash', 'Toothpaste',
                      'Bathing Soap', 'Shampoo', 'Moisturizer'],
              alpha=0.7)
plt.xlabel('Month Number')
plt.ylabel('Sales units in Number')
plt.title('All product sales data using stack plot')
plt.legend(loc='upper left')
plt.grid(True)
plt.show()

# Задание 8
df = pd.read_csv('company_sales_data.csv')

# Задание 8: Создание заготовки для расположения графиков (subplots)
# Создаем фигуру с сеткой 2x3 (2 строки, 3 столбца) для всех 6 продуктов
fig, axes = plt.subplots(2, 3, figsize=(15, 10))
fig.suptitle('Sales Data of All Products per Month', fontsize=16, fontweight='bold')

# Данные о продуктах
products = ['facecream', 'facewash', 'toothpaste', 'bathingsoap', 'shampoo', 'moisturizer']
titles = ['Face Cream', 'Face Wash', 'Toothpaste', 'Bathing Soap', 'Shampoo', 'Moisturizer']
colors = ['red', 'blue', 'green', 'orange', 'purple', 'brown']

# Заполняем каждый подграфик
for i, ax in enumerate(axes.flat):
    ax.plot(df['month_number'], df[products[i]], color=colors[i],
            marker='o', linewidth=2, markersize=6)
    ax.set_title(titles[i], fontsize=12)
    ax.set_xlabel('Month Number', fontsize=10)
    ax.set_ylabel('Sales units', fontsize=10)
    ax.grid(True, linestyle='--', alpha=0.7)
    ax.set_xticks(df['month_number'])

# Настройка отступов, чтобы графики не наезжали друг на друга
plt.tight_layout()
plt.show()