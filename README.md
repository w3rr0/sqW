# sqW database

Autorski interpreter języka SQL oraz lokalny silnik bazodanowy zaimplementowany w C++.
Program pozwala na zarządzanie strukturą tabel oraz manipulację danymi i wykonywanie zapytań.


---

## Kontakt

[Krzysztof Leśniak](https://github.com/krlesniak) \
[krlesniak@student.agh.edu.pl](mailto:krlesniak@student.agh.edu.pl)

[Konrad Mateja](https://github.com/w3rr0) \
[kmateja@student.agh.edu.pl](mailto:kmateja@student.agh.edu.pl)

---

## Założenia programu
* **Ogólne cele:** Implementacja interpretera języka SQL oraz silnika bazodanowego w języku C++. 
Celem projektu jest stworzenie systemu zarządzania danymi, który oferuje pełną obsługę typów (INT, DOUBLE, VARCHAR, BOOL) 
oraz obsługę wartości NULL. Program umożliwia definiowanie struktur tabel (`CREATE`, `DROP`, `ALTER`) oraz manipulację 
rekordami (`INSERT`, `UPDATE`, `DELETE`, `SELECT`).
* **Rodzaj translatora:** Interpreter.
* **Planowany wynik:** Funkcjonalny silnik bazodanowy, który parsuje zapytania SQL do drzewa AST, a następnie wykonuje je na 
autorskiej strukturze danych (klasy Database, Table, Column, Cell). Silnik zapewnia bezpieczeństwo typów, obsługę brakujących 
danych (NULL) oraz prezentację wyników zapytań wraz z ich filtrowaniem i agregacją.
* **Język implementacji:** C++.
* **Realizacja skanera/parsera:** Generatory Flex i Bison.

---

## Dialect
Poniższa tabela zawiera spis tokenów zdefiniowanych w skanerze `lexer.l`.

| Kategoria | Tokeny                                                                                                                         | Opis                                                                                         |
| :--- |:-------------------------------------------------------------------------------------------------------------------------------|:---------------------------------------------------------------------------------------------|
| **Słowa kluczowe (DML/DDL)** | `SELECT`, `FROM`, `WHERE`, `CREATE`, `TABLE`, `INSERT`, `INTO`, `VALUES`, `UPDATE`, `DELETE`, `SET`, `DROP`, `COLUMN`, `ALTER` | Podstawowe komendy zarządzania danymi i strukturą.                                           |
| **Logika i Filtrowanie** | `IN`, `LIKE`, `BETWEEN`, `AND`, `OR`                                                                                           | Operatory warunkowe używane w klauzuli WHERE.                                                |
| **Agregacja i Sortowanie** | `COUNT`, `SUM`, `MIN`, `MAX`, `ORDER`, `BY`, `ASC`, `DESC`, `DISTINCT`, `LIMIT`, `OFFSET`                                      | Funkcje obliczeniowe i sterowanie prezentacją danych.                                        |
| **Typy Danych** | `INT`, `STRING`, `BOOL`, `DOUBLE`, `FLOAT`, `NULL`                                                                             | Obsługiwane typy kolumn w komendzie CREATE TABLE.                                            |
| **Operatory i Symbole** | `STAR` (*), `EQ` (=), `GT` (>), `GE` (>=), `LE` (<=), `LT` (<), `LPAREN` ('('), `RPAREN` (')'), `COMMA` (,), `SEMICOLON` (;)   | Operatory porównania i znaki interpunkcyjne SQL.                                             |
| **Wartości i Nazwy** | `NUMBER`, `ID`, `STR`                                                                                                          | Liczby całkowite, identyfikatory (nazwy) oraz napisy w apostrofach.           |

---

## Gramatyka projektu (Notacja Bison)

### Mechanizm działania i integracja z C++

Integracja składni SQL z silnikiem bazy danych opiera się na wzorcu AST (Abstract Syntax Tree). 
Skaner Flex rozpoznaje słowa kluczowe i symbole, przekazując je do parsera Bison, który dopasowuje je do reguł gramatycznych. 
W momencie rozpoznania poprawnej instrukcji, parser tworzy obiekty klas C++ (np. SelectStmt, InsertStmt), 
wypełniając je danymi z zapytania. Dzięki temu sprawdzanie składni zapytania jest niezależne od wykonywania operacji na danych (execute())


Pełna gramatyka znajduje się w pliku:

**[grammar/parser.y](./grammar/parser.y)**

---

## Technologia i Pakiety Zewnętrzne

Projekt opiera się na nowoczesnych narzędziach programistycznych i bibliotekach:

### Generatory Skanerów/Parserów

- **Flex**  
  Wykorzystywany do generowania skanera (`lexer.l`), który identyfikuje 
  tokeny SQL (słowa kluczowe, literały, operatory).

- **Bison**  
  Generator parserów wykorzystywany do przetwarzania gramatyki
  (`parser.y`) i budowania obiektów instrukcji AST.

### Biblioteki zewnętrzne (zarządzane przez CMake FetchContent)

- **Cereal**  
  Biblioteka *header-only* służąca do zaawansowanej serializacji binarnej 
  bazy danych do pliku `database.bin`.

- **ImGui**  
  Interfejs graficzny wykorzystywany do stworzenia 
  terminala wizualnego, panelu logów, przeglądarki wszystkich tabel 
  w bazie danych oraz wyświetlania tabel z danymi.

- **GLFW i OpenGL**  
  Odpowiadają za tworzenie okien, obsługę wejścia (klawiatura/mysz) 
  oraz renderowanie grafiki.

---

## Instrukcja Obsługi

### Tryb CLI (Terminal)

- Wpisuj polecenia SQL bezpośrednio po `SQL>`.
- Każde polecenie musi kończyć się średnikiem (`;`).
- Można wpisywać kilka zapytań na raz, jedyny warunek, to że
  muszą być oddzielone średnikiem (`;`).

#### Komendy dodatkowe

- `show tables` – wyświetla listę wszystkich tabel w bazie (ignoruje wielkość liter).
- `exit` – bezpiecznie zamyka program.

---

### Tryb GUI (Interfejs)

- **Konsola**  
  Górne okno służy do wpisywania zapytań (obsługuje wiele linii).

- **Skrót klawiszowy**  
  Użyj `CTRL + ENTER` (lub `CMD + ENTER`), aby szybko wykonać zapytanie
  lub kliknij przycsik `Execute Query` pod konsolą główną.

- **Przeglądarka schematów**  
  Prawy panel wyświetla aktualne tabele oraz ich kolumny wraz z typami danych,
  które można rozwijać z zwijać z powrotem.

- **Historia**  
  Środkowy panel logów wyświetla historię zapytań z kolorowaniem składni SQL.

- **Tabela wyników (panel pod historią logów)**  
  Dolna część interfejsu zawiera tabelę wyników zapytań SQL. Wyświetla ona 
  dane zwrócone przez wykonane zapytanie w formie wierszy i kolumn.

---

## Przykład Użycia

Poniżej znajdują się przykładowe zapytania każdego typu tworzące bazę i manipulujące danymi:

```sql
-- 1. Tworzenie tabeli z kompletem typów danych
CREATE TABLE klienci (id INT, imie STRING, nazwisko STRING, miasto STRING, aktywny BOOL);

-- 2. Dodawanie rekordów do bazy
INSERT INTO klienci VALUES (1, 'Jan', 'Kowalski', 'Krakow', 1),
                           (2, 'Anna', 'Nowak', 'Warszawa', 1),
                           (3, 'Piotr', 'Zielinski', 'Krakow', 0),
                           (4, 'Katarzyna', 'Mazurek', 'Poznan', 1),
                           (5, 'Michal', 'Wisniewski', 'Gdansk', 1),
                           (6, 'Barbara', 'Wojcik', 'Katowice', 0);

-- 3. Aktualizacja danych
UPDATE klienci SET imie = 'Krzysztof' WHERE id = 1;

-- 4. Zapytanie SELECT z filtrowaniem, sortowaniem i limitami
SELECT imie, nazwisko, miasto FROM klienci
WHERE imie LIKE '%a' AND id BETWEEN 1 AND 4
ORDER BY id DESC LIMIT 1;

-- 5. Usuwanie kolumny z istniejącej tabeli
ALTER TABLE klienci DROP COLUMN aktywny;

-- 6. Dodawanie nowej kolumny (wypełni się domyślnie wartościami NULL)
ALTER TABLE klienci ADD premia NUMERIC;

-- 7. Przypisanie premii wybranym pracownikom przed filtrowaniem NULLi
UPDATE klienci SET premia = 250.50 WHERE id = 2;
UPDATE klienci SET premia = 500.00 WHERE id = 4;

-- 7a. Test filtrowania IS NULL / IS NOT NULL
SELECT imie, nazwisko, premia FROM klienci WHERE premia IS NOT NULL;

-- 8. Usuwanie rekordów z tabeli na podstawie listy miast
DELETE FROM klienci WHERE miasto IN ('Krakow', 'Warszawa', 'Katowice', 'Poznan');

-- 9. Wyświetlenie tego, co zostało w bazie
SELECT * FROM klienci;

-- 10. Czyszczenie bazy - usunięcie tabeli
DROP TABLE klienci;
```
---

## Dodatkowe Informacje

- **Serializacja**  
  Baza danych jest automatycznie zapisywana do pliku binarnego 
  `database.bin` po każdej udanej operacji modyfikującej 
  (`INSERT`, `UPDATE`, itp.).

- **Bezpieczeństwo**  
  Silnik sprawdza zgodność typów przy wstawianiu danych oraz weryfikuje
  istnienie kolumn przed wykonaniem operacji `ALTER` lub `DROP`.

---

## Prerequisites

To build this project, you need to have the following tools installed:

* **Bison**
* **Flex**
* **GLFW**

  ```shell
  sudo apt install bison flex libglfw3-dev    # Linux (Debian)
  ```

  ```shell
  brew install bison flex glfw                # MacOS
  ```
  
---

## Getting Started

Follow these steps to build and run the application from the root directory.

1. **Configure the project** \
   Create a build directory and configure the project using CMake:
   ```shell
   cmake -S . -B build
   ```
2. **Build the application** \
   Compile the source code:
   ```shell
    cmake --build build
   ```
3. **Run the application** \
   After a successful build, run the executable.
        
   CLI
   ```shell
   ./build/sqW                    # Linux / MacOS
   ```
   ```powershell
   .\build\Debug\sqW.exe          # Windows
   ```
   GUI
   ```shell
   ./build/sqW --gui              # Linux / MacOS
   ```
   ```powershell
   .\build\Debug\sqW.exe --gui    # Windows
   ```

---

## License

The program is licensed under the MIT license.
Details in the [License](LICENSE) file
