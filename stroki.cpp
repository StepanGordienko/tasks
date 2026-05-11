/*
 * Вариант 4 — «Символы и строки»
 * Задание 1: удалить из строки русские согласные из заданного набора (UTF-8).
 * Задание 2: N самых длинных уникальных слов из text.txt без букв из данного слова.
 *
 * input.txt:
 *   строка 1 — текст для задания 1 (до 100 символов в задании; буфер больше)
 *   строка 2 — N
 *   строка 3 — «данное слово» (запрещённые буквы)
 * text.txt — русский текст для задания 2
 * result.txt — вывод обоих заданий
 */

#include <fstream>
#include <iomanip>
#include <iostream>

namespace {

    constexpr int MAX_LINE = 512;
    constexpr int MAX_WORD_LEN = 256;
    constexpr int MAX_WORDS = 2000;
    constexpr int MAX_TEXT = 512 * 1024;
    constexpr int MAX_FORB = 64;

    int utf8_char_len(unsigned char c) {
        if (c < 0x80) return 1;
        if ((c & 0xE0) == 0xC0) return 2;
        if ((c & 0xF0) == 0xE0) return 3;
        if ((c & 0xF8) == 0xF0) return 4;
        return 1;
    }

    // Кириллица UTF-8 (2 байта) -> в out строковые нижним регистром; вернуть 2 или 0
    int read_cyrillic_unit(const char* p, unsigned char out[2]) {
        unsigned char c0 = static_cast<unsigned char>(p[0]);
        if (c0 == 0) return 0;
        int L = utf8_char_len(c0);
        if (L != 2) return 0;
        unsigned char c1 = static_cast<unsigned char>(p[1]);
        if (c1 == 0) return 0;
        bool ok = false;
        if (c0 == 0xD0) {
            if ((c1 >= 0x90 && c1 <= 0xBF) || c1 == 0x81) ok = true;
        }
        else if (c0 == 0xD1) {
            if ((c1 >= 0x80 && c1 <= 0x8F) || c1 == 0x91) ok = true;
        }
        if (!ok) return 0;
        out[0] = c0;
        out[1] = c1;
        if (c0 == 0xD0 && c1 == 0x81) { // Ё
            out[0] = 0xD1;
            out[1] = 0x91;
            return 2;
        }
        if (c0 == 0xD0 && c1 >= 0x90 && c1 <= 0xAF) { // А-Я
            out[0] = 0xD0;
            out[1] = static_cast<unsigned char>(c1 + 0x20);
            return 2;
        }
        return 2;
    }

    bool is_ascii_digit(unsigned char c) { return c >= '0' && c <= '9'; }
    bool is_ascii_letter(unsigned char c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }

    bool is_word_start_char(const char* p) {
        unsigned char c = static_cast<unsigned char>(*p);
        if (is_ascii_digit(c) || is_ascii_letter(c)) return true;
        unsigned char low[2];
        return read_cyrillic_unit(p, low) != 0;
    }

    bool is_word_body_char(const char* p) {
        unsigned char c = static_cast<unsigned char>(*p);
        if (c == '-' || is_ascii_digit(c) || is_ascii_letter(c)) return true;
        unsigned char low[2];
        return read_cyrillic_unit(p, low) != 0;
    }

    // Согласные по условию (строчные UTF-8)
    static const unsigned char CONS_BYTES[][2] = {
        {0xD0, 0xB1}, {0xD0, 0xB2}, {0xD0, 0xB3}, {0xD0, 0xB4}, {0xD0, 0xB6},
        {0xD0, 0xB7}, {0xD0, 0xB9}, {0xD0, 0xBA}, {0xD0, 0xBB}, {0xD0, 0xBC},
        {0xD0, 0xBD}, {0xD0, 0xBF}, {0xD1, 0x80}, {0xD1, 0x81}, {0xD1, 0x82},
        {0xD1, 0x84}, {0xD1, 0x85}, {0xD1, 0x86}, {0xD1, 0x87}, {0xD1, 0x88},
        {0xD1, 0x89},
    };

    bool unit_in_set(const unsigned char u[2], const unsigned char set[][2], int n) {
        for (int i = 0; i < n; ++i)
            if (u[0] == set[i][0] && u[1] == set[i][1]) return true;
        return false;
    }

    bool is_russian_consonant_utf8(const char* p) {
        unsigned char u[2];
        if (read_cyrillic_unit(p, u) == 0) return false;
        const int nc = static_cast<int>(sizeof(CONS_BYTES) / sizeof(CONS_BYTES[0]));
        return unit_in_set(u, CONS_BYTES, nc);
    }

    int my_strlen(const char* s) {
        int n = 0;
        while (s[n] != '\0') ++n;
        return n;
    }

    int utf8_unit_count(const char* s) {
        int cnt = 0;
        for (int i = 0; s[i] != '\0';) {
            unsigned char c = static_cast<unsigned char>(s[i]);
            int L = utf8_char_len(c);
            if (L < 1) L = 1;
            ++cnt;
            i += L;
        }
        return cnt;
    }

    // Индекс начала последнего UTF-8 символа в [0, n)
    int utf8_last_unit_start(const char* raw, int n) {
        if (n <= 0) return -1;
        int pos = n - 1;
        while (pos > 0 && (static_cast<unsigned char>(raw[pos]) & 0xC0) == 0x80) --pos;
        return pos;
    }

    bool codepoint_is_letter(const char* raw, int start) {
        unsigned char c = static_cast<unsigned char>(raw[start]);
        int L = utf8_char_len(c);
        if (L == 1 && is_ascii_letter(c)) return true;
        if (L == 2) {
            unsigned char u[2];
            return read_cyrillic_unit(raw + start, u) != 0;
        }
        return false;
    }

    void task1_remove_consonants(const char* input, char* out) {
        int o = 0;
        for (int i = 0; input[i] != '\0';) {
            unsigned char c = static_cast<unsigned char>(input[i]);
            int L = utf8_char_len(c);
            if (L == 2 && is_russian_consonant_utf8(input + i)) {
                i += L;
                continue;
            }
            for (int k = 0; k < L && input[i + k] != '\0'; ++k) out[o++] = input[i + k];
            i += L;
        }
        out[o] = '\0';
    }

    int collect_letter_units(const char* word, unsigned char letters[][2], int maxLetters) {
        int count = 0;
        for (int i = 0; word[i] != '\0';) {
            unsigned char c = static_cast<unsigned char>(word[i]);
            int L = utf8_char_len(c);
            unsigned char u[2];
            if (L == 2 && read_cyrillic_unit(word + i, u) != 0) {
                bool dup = false;
                for (int j = 0; j < count; ++j)
                    if (letters[j][0] == u[0] && letters[j][1] == u[1]) dup = true;
                if (!dup && count < maxLetters) {
                    letters[count][0] = u[0];
                    letters[count][1] = u[1];
                    ++count;
                }
            }
            else if (L == 1 && is_ascii_letter(c)) {
                unsigned char ch = static_cast<unsigned char>((c >= 'A' && c <= 'Z') ? (c - 'A' + 'a') : c);
                bool dup = false;
                for (int j = 0; j < count; ++j)
                    if (letters[j][0] == ch && letters[j][1] == 0) dup = true;
                if (!dup && count < maxLetters) {
                    letters[count][0] = ch;
                    letters[count][1] = 0;
                    ++count;
                }
            }
            i += L;
        }
        return count;
    }

    bool word_contains_forbidden(const char* word, unsigned char forb[][2], int nf) {
        for (int i = 0; word[i] != '\0';) {
            unsigned char c = static_cast<unsigned char>(word[i]);
            int L = utf8_char_len(c);
            unsigned char u[2];
            if (L == 2 && read_cyrillic_unit(word + i, u) != 0) {
                for (int j = 0; j < nf; ++j)
                    if (forb[j][1] != 0 && forb[j][0] == u[0] && forb[j][1] == u[1]) return true;
            }
            else if (L == 1 && is_ascii_letter(c)) {
                unsigned char ch = static_cast<unsigned char>((c >= 'A' && c <= 'Z') ? (c - 'A' + 'a') : c);
                for (int j = 0; j < nf; ++j)
                    if (forb[j][1] == 0 && forb[j][0] == ch) return true;
            }
            i += L;
        }
        return false;
    }

    // writable raw buffer: удаляем хвостовые ASCII-цифры, проверяем что оканчивается буквой, заполняем norm в нижнем регистре
    bool normalize_word_token(char* raw, char* norm, int normMax) {
        int n = my_strlen(raw);
        while (n > 0 && is_ascii_digit(static_cast<unsigned char>(raw[n - 1]))) --n;
        raw[n] = '\0';
        if (n == 0) return false;
        int ls = utf8_last_unit_start(raw, n);
        if (ls < 0 || !codepoint_is_letter(raw, ls)) return false;

        int o = 0;
        for (int i = 0; raw[i] != '\0' && o + 8 < normMax;) {
            unsigned char c = static_cast<unsigned char>(raw[i]);
            int L = utf8_char_len(c);
            if (L == 2) {
                unsigned char u[2];
                if (read_cyrillic_unit(raw + i, u) != 0) {
                    norm[o++] = static_cast<char>(u[0]);
                    norm[o++] = static_cast<char>(u[1]);
                }
                else {
                    norm[o++] = raw[i];
                    norm[o++] = raw[i + 1];
                }
                i += 2;
                continue;
            }
            char ch = raw[i];
            if (ch >= 'A' && ch <= 'Z') ch = static_cast<char>(ch - 'A' + 'a');
            norm[o++] = ch;
            i += L;
        }
        norm[o] = '\0';
        return true;
    }

    bool words_equal(const char* a, const char* b) {
        int i = 0;
        for (;; ++i) {
            if (a[i] != b[i]) return false;
            if (a[i] == '\0') return true;
        }
    }

    void copy_str(char* dst, const char* src, int dstCap) {
        int i = 0;
        for (; src[i] != '\0' && i + 1 < dstCap; ++i) dst[i] = src[i];
        dst[i] = '\0';
    }

    bool read_line(std::istream& in, char* buf, int cap) {
        if (!in.getline(buf, cap)) return false;
        int n = my_strlen(buf);
        if (n > 0 && buf[n - 1] == '\r') buf[n - 1] = '\0';
        return true;
    }

    // Сдвиг строки, если в начале UTF-8 BOM
    void strip_utf8_bom(char* s) {
        if (static_cast<unsigned char>(s[0]) == 0xEF && static_cast<unsigned char>(s[1]) == 0xBB &&
            static_cast<unsigned char>(s[2]) == 0xBF) {
            int i = 3;
            int j = 0;
            while (s[i] != '\0') s[j++] = s[i++];
            s[j] = '\0';
        }
    }

    int parse_positive_int(const char* s) {
        int v = 0;
        for (int i = 0; s[i] != '\0'; ++i) {
            if (s[i] < '0' || s[i] > '9') return 0;
            v = v * 10 + (s[i] - '0');
            if (v > 1000000) return 1000000;
        }
        return v;
    }

    void print_ascii_row(char ch) {
        std::cout << std::left << std::setw(10) << ch << std::right << static_cast<int>(static_cast<unsigned char>(ch))
            << '\n';
    }

    void print_utf8_row(const char* letter, int codepoint) {
        std::cout << std::left << std::setw(10) << letter << std::right << "U+";
        std::cout << std::uppercase << std::hex << std::setw(4) << std::setfill('0') << codepoint;
        std::cout << std::dec << std::nouppercase << std::setfill(' ') << '\n';
    }

    void print_point0_table() {
        static const char* RU_UPPER[] = { "А", "Б", "В", "Г", "Д", "Е", "Ё", "Ж", "З", "И", "Й", "К",
                                         "Л", "М", "Н", "О", "П", "Р", "С", "Т", "У", "Ф", "Х", "Ц",
                                         "Ч", "Ш", "Щ", "Ъ", "Ы", "Ь", "Э", "Ю", "Я" };
        static const char* RU_LOWER[] = { "а", "б", "в", "г", "д", "е", "ё", "ж", "з", "и", "й", "к",
                                         "л", "м", "н", "о", "п", "р", "с", "т", "у", "ф", "х", "ц",
                                         "ч", "ш", "щ", "ъ", "ы", "ь", "э", "ю", "я" };
        static const int RU_UPPER_CP[] = { 0x0410, 0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0401, 0x0416, 0x0417,
                                          0x0418, 0x0419, 0x041A, 0x041B, 0x041C, 0x041D, 0x041E, 0x041F, 0x0420,
                                          0x0421, 0x0422, 0x0423, 0x0424, 0x0425, 0x0426, 0x0427, 0x0428, 0x0429,
                                          0x042A, 0x042B, 0x042C, 0x042D, 0x042E, 0x042F };
        static const int RU_LOWER_CP[] = { 0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0451, 0x0436, 0x0437,
                                          0x0438, 0x0439, 0x043A, 0x043B, 0x043C, 0x043D, 0x043E, 0x043F, 0x0440,
                                          0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447, 0x0448, 0x0449,
                                          0x044A, 0x044B, 0x044C, 0x044D, 0x044E, 0x044F };
        static const char PUNCT[] = ".,;:!?-()[]{}\"'`";

        std::cout << "Пункт 0. Таблица Буква/Код\n";
        std::cout << "Символ    Код\n";

        std::cout << "\nАнглийский алфавит (верхний регистр):\n";
        for (char c = 'A'; c <= 'Z'; ++c) print_ascii_row(c);

        std::cout << "\nАнглийский алфавит (нижний регистр):\n";
        for (char c = 'a'; c <= 'z'; ++c) print_ascii_row(c);

        std::cout << "\nРусский алфавит (верхний регистр):\n";
        for (int i = 0; i < 33; ++i) print_utf8_row(RU_UPPER[i], RU_UPPER_CP[i]);

        std::cout << "\nРусский алфавит (нижний регистр):\n";
        for (int i = 0; i < 33; ++i) print_utf8_row(RU_LOWER[i], RU_LOWER_CP[i]);

        std::cout << "\nЦифры:\n";
        for (char c = '0'; c <= '9'; ++c) print_ascii_row(c);

        std::cout << "\nСтандартные знаки препинания:\n";
        for (int i = 0; PUNCT[i] != '\0'; ++i) print_ascii_row(PUNCT[i]);
        std::cout << '\n';
    }

} // namespace

int main() {
    print_point0_table();

    char line1[MAX_LINE];
    char lineN[MAX_LINE];
    char givenWord[MAX_LINE];
    char textBuf[MAX_TEXT];
    char words[MAX_WORDS][MAX_WORD_LEN];
    int wordLenUnits[MAX_WORDS];
    int wordCount = 0;

    std::ifstream fin("input.txt");
    if (!fin) {
        std::cerr << "Не удалось открыть input.txt\n";
        return 1;
    }
    if (!read_line(fin, line1, MAX_LINE)) line1[0] = '\0';
    if (!read_line(fin, lineN, MAX_LINE)) lineN[0] = '0';
    if (!read_line(fin, givenWord, MAX_LINE)) givenWord[0] = '\0';
    fin.close();

    strip_utf8_bom(line1);
    strip_utf8_bom(lineN);
    strip_utf8_bom(givenWord);

    unsigned char forb[MAX_FORB][2];
    int nf = collect_letter_units(givenWord, forb, MAX_FORB);

    int N = parse_positive_int(lineN);
    if (N < 0) N = 0;

    char t1out[MAX_LINE];
    task1_remove_consonants(line1, t1out);

    std::ifstream tfin("text.txt", std::ios::binary);
    if (!tfin) {
        std::cerr << "Не удалось открыть text.txt\n";
    }
    else {
        tfin.read(textBuf, MAX_TEXT - 1);
        std::streamsize got = tfin.gcount();
        textBuf[got] = '\0';
        strip_utf8_bom(textBuf);
        tfin.close();
    }

    // Разбор слов
    for (int i = 0; textBuf[i] != '\0';) {
        if (!is_word_start_char(textBuf + i)) {
            ++i;
            continue;
        }
        int start = i;
        while (textBuf[i] != '\0' && is_word_body_char(textBuf + i)) {
            unsigned char c = static_cast<unsigned char>(textBuf[i]);
            i += utf8_char_len(c);
        }
        int lenSeg = i - start;
        if (lenSeg <= 0 || lenSeg >= MAX_WORD_LEN) continue;

        char raw[MAX_WORD_LEN];
        for (int k = 0; k < lenSeg; ++k) raw[k] = textBuf[start + k];
        raw[lenSeg] = '\0';

        char norm[MAX_WORD_LEN];
        if (!normalize_word_token(raw, norm, MAX_WORD_LEN)) continue;
        if (word_contains_forbidden(norm, forb, nf)) continue;

        bool dup = false;
        for (int w = 0; w < wordCount; ++w) {
            if (words_equal(words[w], norm)) {
                dup = true;
                break;
            }
        }
        if (dup) continue;
        if (wordCount >= MAX_WORDS) break;

        copy_str(words[wordCount], norm, MAX_WORD_LEN);
        wordLenUnits[wordCount] = utf8_unit_count(norm);
        ++wordCount;
    }

    // Сортировка по убыванию длины (пузырёк; слов немного)
    for (int a = 0; a < wordCount; ++a) {
        for (int b = a + 1; b < wordCount; ++b) {
            if (wordLenUnits[b] > wordLenUnits[a]) {
                int tmpL = wordLenUnits[a];
                wordLenUnits[a] = wordLenUnits[b];
                wordLenUnits[b] = tmpL;
                char tmpW[MAX_WORD_LEN];
                copy_str(tmpW, words[a], MAX_WORD_LEN);
                copy_str(words[a], words[b], MAX_WORD_LEN);
                copy_str(words[b], tmpW, MAX_WORD_LEN);
            }
        }
    }

    std::ofstream fout("result.txt");
    if (!fout) {
        std::cerr << "Не удалось записать result.txt\n";
        return 1;
    }

    fout << "Задание 1:\n";
    fout << t1out << "\n\n";
    fout << "Задание 2:\n";
    int outN = N;
    if (outN > wordCount) outN = wordCount;
    for (int k = 0; k < outN; ++k) fout << words[k] << '\n';

    fout.close();
    return 0;
}
