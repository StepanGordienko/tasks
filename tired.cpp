#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <windows.h>

using namespace std;

char utf8ToCp1251(const char* utf8, int& pos) {
    unsigned char c = (unsigned char)utf8[pos];

    if (c < 128) {
        pos++;
        return (char)c;
    }

    if ((c & 0xE0) == 0xC0) {
        unsigned char c2 = (unsigned char)utf8[pos + 1];
        int code = ((c & 0x1F) << 6) | (c2 & 0x3F);

        if (code >= 1040 && code <= 1103) {
            pos += 2;
            return (char)(code - 848);
        }
        if (code == 1025) { pos += 2; return (char)168; }
        if (code == 1105) { pos += 2; return (char)184; }
    }

    pos++;
    return '?';
}

bool isRussianLetter(char c) {
    int code = (int)(unsigned char)c;
    return (code >= 192 && code <= 255) || code == 168 || code == 184;
}

bool isEnglishLetter(char c) {
    int code = (int)(unsigned char)c;
    return (code >= 65 && code <= 90) || (code >= 97 && code <= 122);
}

bool isAnyLetter(char c) {
    return isRussianLetter(c) || isEnglishLetter(c);
}

char toLowerRu(char c) {
    int code = (int)(unsigned char)c;
    if (code >= 192 && code <= 223) return (char)(code + 32);
    if (code == 168) return (char)184;
    if (code >= 65 && code <= 90) return (char)(code + 32);
    return c;
}

bool isConsonant(char c) {
    int code = (int)(unsigned char)toLowerRu(c);
    return code == 225 || code == 226 || code == 227 || code == 228 || code == 230 ||
        code == 231 || code == 233 || code == 234 || code == 235 || code == 236 ||
        code == 237 || code == 239 || code == 240 || code == 241 || code == 242 ||
        code == 244 || code == 245 || code == 246 || code == 247 || code == 248 ||
        code == 249;
}

bool isLetter(char c) {
    int code = (int)(unsigned char)c;
    return isRussianLetter(c) || (code >= 65 && code <= 90) || (code >= 97 && code <= 122);
}

int myStrLen(const char* s) {
    int i = 0;
    while (s[i]) i++;
    return i;
}

void myStrCpy(char* dst, const char* src) {
    int i = 0;
    while ((dst[i] = src[i]) != '\0') i++;
}

bool strEqual(const char* a, const char* b) {
    int i = 0;
    while (a[i] && b[i] && a[i] == b[i]) i++;
    return a[i] == 0 && b[i] == 0;
}

struct Word {
    char text[256];
    int  length;
};

void punkt0() {
    cout << "\nПункт 0\n";
    cout << "Буква\tКод\n";

    const char* eng_upper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char* eng_lower = "abcdefghijklmnopqrstuvwxyz";
    const char* digits = "0123456789";
    const char* punct = ".,;:!?-()[]{}\"'`/\\@#$%^&*+=<>|~_ ";

    cout << "\nАнглийские заглавные\n";
    for (int i = 0; eng_upper[i]; i++)
        cout << eng_upper[i] << "\t\t" << (int)(unsigned char)eng_upper[i] << "\n";

    cout << "\nАнглийские строчные\n";
    for (int i = 0; eng_lower[i]; i++)
        cout << eng_lower[i] << "\t\t" << (int)(unsigned char)eng_lower[i] << "\n";

    cout << "\nЦифры\n";
    for (int i = 0; digits[i]; i++)
        cout << digits[i] << "\t\t" << (int)(unsigned char)digits[i] << "\n";

    cout << "\nЗнаки препинания\n";
    for (int i = 0; punct[i]; i++)
        cout << punct[i] << "\t\t" << (int)(unsigned char)punct[i] << "\n";

    cout << "\nРусские заглавные\n";
    for (int code = 192; code <= 223; code++)
        cout << (char)(unsigned char)code << "\t\t" << code << "\n";
    cout << (char)(unsigned char)168 << "\t\t" << 168 << "  (Ё)\n";

    cout << "\nРусские строчные\n";
    for (int code = 224; code <= 255; code++)
        cout << (char)(unsigned char)code << "\t\t" << code << "\n";
    cout << (char)(unsigned char)184 << "\t\t" << 184 << "  (ё)\n";
}

void task1() {
    char str[101];
    cout << "Введите строку: ";
    cin.ignore();
    cin.getline(str, 101);

    char result[101];
    int j = 0;
    for (int i = 0; str[i]; i++)
        if (!isConsonant(str[i]))
            result[j++] = str[i];
    result[j] = '\0';

    cout << "Результат: " << result << "\n";
}

bool hasForbiddenLetter(const char* word, const char* filter) {
    for (int i = 0; word[i]; i++) {
        char c = toLowerRu(word[i]);
        for (int j = 0; filter[j]; j++) {
            if (c == toLowerRu(filter[j])) {
                return true;
            }
        }
    }
    return false;
}

void cleanWord(char* word) {
    int len = myStrLen(word);
    if (len == 0) return;

    int start = 0;
    while (start < len && !isAnyLetter(word[start])) {
        start++;
    }

    int end = len - 1;
    while (end >= start && !isAnyLetter(word[end])) {
        end--;
    }

    if (start > end) {
        word[0] = '\0';
        return;
    }

    char temp[256];
    int j = 0;
    for (int i = start; i <= end; i++) {
        temp[j++] = word[i];
    }
    temp[j] = '\0';

    myStrCpy(word, temp);
}

bool isEqualIgnoreCase(const char* a, const char* b) {
    char tempA[256], tempB[256];
    myStrCpy(tempA, a);
    myStrCpy(tempB, b);

    for (int i = 0; tempA[i]; i++) tempA[i] = toLowerRu(tempA[i]);
    for (int i = 0; tempB[i]; i++) tempB[i] = toLowerRu(tempB[i]);

    return strEqual(tempA, tempB);
}

void task2() {
    cout << "\nЗадание 2\n";

    ifstream fin("input.txt");
    if (!fin) {
        cout << "Ошибка: не удалось открыть input.txt\n";
        return;
    }

    int N;
    fin >> N;
    fin.ignore();

    char filterWord[256];
    fin.getline(filterWord, 256);
    fin.close();

    ifstream ftxt("text.txt", ios::binary);
    if (!ftxt) {
        cout << "Ошибка: не удалось открыть text.txt\n";
        return;
    }

    char utf8text[20000];
    ftxt.read(utf8text, 20000);
    int utf8len = ftxt.gcount();
    utf8text[utf8len] = '\0';
    ftxt.close();

    char text[20000];
    int textLen = 0;
    int pos = 0;
    while (pos < utf8len) {
        char c = utf8ToCp1251(utf8text, pos);
        if (textLen < 19999) {
            text[textLen++] = c;
        }
    }
    text[textLen] = '\0';

    Word words[500];
    int wordCount = 0;

    int i = 0;
    while (i < textLen) {
        while (i < textLen && !isAnyLetter(text[i]) && !(text[i] >= '0' && text[i] <= '9')) {
            i++;
        }

        if (i >= textLen) break;

        int wordStart = i;

        while (i < textLen && (isAnyLetter(text[i]) || (text[i] >= '0' && text[i] <= '9') ||
            (text[i] == '-' && i > wordStart && i + 1 < textLen && isAnyLetter(text[i + 1])))) {
            i++;
        }

        int wordEnd = i - 1;

        char rawWord[256];
        int j = 0;
        for (int k = wordStart; k <= wordEnd; k++) {
            rawWord[j++] = text[k];
        }
        rawWord[j] = '\0';

        cleanWord(rawWord);

        int len = myStrLen(rawWord);

        if (len > 0 && isAnyLetter(rawWord[0])) {
            if (!hasForbiddenLetter(rawWord, filterWord)) {
                bool isDuplicate = false;
                for (int k = 0; k < wordCount; k++) {
                    if (isEqualIgnoreCase(words[k].text, rawWord)) {
                        isDuplicate = true;
                        break;
                    }
                }

                if (!isDuplicate) {
                    myStrCpy(words[wordCount].text, rawWord);
                    words[wordCount].length = len;
                    wordCount++;
                }
            }
        }
    }

    if (wordCount == 0) {
        cout << "Нет подходящих слов.\n";
        ofstream fout("result.txt");
        fout << "Задача 2:\nНет слов\n";
        fout.close();
        return;
    }

    for (int i = 0; i < wordCount - 1; i++) {
        for (int j = 0; j < wordCount - i - 1; j++) {
            if (words[j].length < words[j + 1].length) {
                Word temp = words[j];
                words[j] = words[j + 1];
                words[j + 1] = temp;
            }
        }
    }

    int resultCount;
    if (N < wordCount) {
        resultCount = N;
    }
    else {
        resultCount = wordCount;
    }

    ofstream fout("result.txt");
    for (int i = 0; i < resultCount; i++) {
        fout << words[i].text << "\n";
    }
    fout.close();
}

int main() {
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);

    cout << "Выберите задачу(0-2): ";
    int choice;
    cin >> choice;

    switch (choice) {
    case 0: punkt0(); break;
    case 1: task1();  break;
    case 2: task2();  break;
    default: cout << "Неверный выбор\n";
    }

    return 0;
}