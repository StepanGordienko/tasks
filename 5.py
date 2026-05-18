import time
from datetime import datetime
from functools import wraps


def log_decorator(func):
    @wraps(func)
    def wrapper(*args, **kwargs):
        start_time = time.time()
        start_datetime = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

        with open("log.txt", "a", encoding="utf-8") as log_file:
            log_file.write(f"[{start_datetime}] Функция '{func.__name__}' вызвана с аргументами: {args}\n")

        result = func(*args, **kwargs)

        end_time = time.time()
        execution_time = end_time - start_time
        end_datetime = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

        with open("log.txt", "a", encoding="utf-8") as log_file:
            log_file.write(
                f"[{end_datetime}] Функция '{func.__name__}' завершена. Время выполнения: {execution_time:.1f} сек.\n")

        return result

    return wrapper


@log_decorator
def calculate(a, b, operation):
    if operation == '+':
        return a + b
    elif operation == '-':
        return a - b
    elif operation == '*':
        return a * b
    elif operation == '/':
        return a / b
    else:
        raise ValueError("Неподдерживаемая операция")

calculate(10, 5, '+')
calculate(8, 2, '*')
calculate(9, 3, '/')

def cache_decorator(func):
    cache = {}
    @wraps(func)
    def wrapper(*args, **kwargs):
        key = (args, tuple(sorted(kwargs.items())))

        if key in cache:
            print(f"Возвращаем из кэша для аргументов {args}")
            return cache[key]

        print(f"Вычисляем для аргументов {args}")
        result = func(*args, **kwargs)
        cache[key] = result
        return result

    return wrapper


@cache_decorator
def fibonacci(n):
    if n <= 1:
        return n
    return fibonacci(n - 1) + fibonacci(n - 2)


print("Первый вызов:")
print(fibonacci(10))

print("\nВторой вызов (должен быть из кэша):")
print(fibonacci(10))

print("\nВызов с другим аргументом:")
print(fibonacci(5))