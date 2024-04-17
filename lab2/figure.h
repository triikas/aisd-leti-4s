#include <iostream>
#include <list>

class OutOfScreen : std::exception{
public:
    int x, y;

    OutOfScreen(int a = 0, int b = 0) : x(a), y(b) {}
    void show() {
        std::cerr << "Element out of screen: (" << x << "," << y << ")\n";
    }
};

class CantBeMoved : std::exception{
public:
    int x, y;

    CantBeMoved(int a = 0, int b = 0) : x(a), y(b) {}
    void show() {
        std::cerr << "Element can't be moved: (" << x << "," << y << ")\n";
    }
};

class point { // Точка на экране
  public:
    int x, y;

    point(int a = 0, int b = 0) : x(a), y(b) {
//        if (x > 150 || x < 0 || x > 50 || x < 0) {throw OutOfScreen(x, y);}
    }
};
// Набор утилит для работы с экраном
void put_point(int a, int b); // Вывод точки (2 варианта)
void put_point(point p) { put_point(p.x, p.y); } //
void put_line(int, int, int, int); // Вывод линии (2 варианта)
void put_line(point a, point b) { put_line(a.x, a.y, b.x, b.y); }
void screen_init();    // Создание экрана
void screen_destroy(); // Удаление
void screen_refresh(); // Обновление
void screen_clear();   // Очистка
char screen[50][150];
enum color { black = '*', white = '.' };
void screen_init() {
    for (auto y = 0; y < 50; ++y)
        for (auto &x : screen[y])
            x = white;
}
void screen_destroy() {
    for (auto y = 0; y < 50; ++y)
        for (auto &x : screen[y])
            x = black;
}
bool on_screen(int a, int b) // проверка попадания точки на экран
{
    return 0 <= a && a < 150 && 0 <= b && b < 50;
}
void put_point(int a, int b) {
    if (on_screen(a, b)) {
        screen[b][a] = black;
    } else {
//        if (a > 150) {
//            a = 150;
//        } else if (a < 0) {
//            a = 0;
//        } else if (b > 50) {
//            b = 50;
//        } else if (b < 0) {
//            b = 0;
//        }
//        screen[b][a] = black;
        throw OutOfScreen(a, b);
    }

}
void put_line(int x0, int y0, int x1, int y1) {
    int dx = 1;
    int a = x1 - x0;
    if (a < 0)
        dx = -1, a = -a;
    int dy = 1;
    int b = y1 - y0;
    if (b < 0)
        dy = -1, b = -b;
    int two_a = 2 * a;
    int two_b = 2 * b;
    int xcrit = -b + two_a;
    int eps = 0;
    for (;;) { // Формирование прямой линии по точкам
        put_point(x0, y0);
        if (x0 == x1 && y0 == y1)
            break;
        if (eps <= xcrit)
            x0 += dx, eps += two_b;
        if (eps >= a || a < b)
            y0 += dy, eps -= two_a;
    }
}
void screen_clear() { screen_init(); } // Очистка экрана
void screen_refresh()                  // Обновление экрана
{
    for (int y = 50 - 1; 0 <= y; --y) { // с верхней строки до нижней
        for (auto x : screen[y]) // от левого столбца до правого
            std::cout << x;
        std::cout << '\n';
    }
}
struct shape { // Виртуальный базовый класс «фигура»
    static std::list<shape *> shapes; // Список фигур (один на все фигуры!)
    shape() { shapes.push_back(this); } // Фигура присоединяется к списку
    virtual point north() const = 0; // Точки для привязки
    virtual point south() const = 0;
    virtual point east() const = 0;
    virtual point west() const = 0;
    virtual point neast() const = 0;
    virtual point seast() const = 0;
    virtual point nwest() const = 0;
    virtual point swest() const = 0;
    virtual void draw() = 0;         // Рисование
    virtual void move(int, int) = 0; // Перемещение
    virtual void resize(double) = 0; // Изменение размера
    // virtual ~shape() { shapes.remove(this); } // Деструктор
};
std::list<shape *> shape::shapes; // Размещение списка фигур
void shape_refresh() // Перерисовка всех фигур на экране
{
    screen_clear();
    for (auto p : shape ::shapes)
        p->draw(); // Динамическое связывание!!!
    screen_refresh();
}
class rotatable : virtual public shape { // Фигуры, пригодные к повороту
  public:
    virtual void rotate_left() = 0;  // Повернуть влево
    virtual void rotate_right() = 0; // Повернуть вправо
};
class reflectable : virtual public shape { // Фигуры, пригодные
  public: // к зеркальному отражению
    virtual void flip_horisontally() = 0; // Отразить горизонтально
    virtual void flip_vertically() = 0; // Отразить вертикально
};
class line : public shape { // ==== Прямая линия ====
                            /* отрезок прямой ["w", "e"].
                               north( ) определяет точку «выше центра отрезка и так далеко
                               на север, как самая его северная точка», и т. п. */
  protected:
    point w, e;

  public:
    line(point a, point b)
        : w(a), e(b){}; // Произвольная линия (по двум
                        // точкам)
    line(point a, int L)
        : w(point(a.x + L - 1, a.y)), e(a){}; // Горизонтальная линия
    point north() const {
        return point((w.x + e.x) / 2, e.y < w.y ? w.y : e.y);
    }
    point south() const {
        return point((w.x + e.x) / 2, e.y < w.y ? e.y : w.y);
    }
    point east() const { return point(e.x < w.x ? w.x : e.x, (w.y + e.y) / 2); }
    point west() const { return point(e.x < w.x ? e.x : w.x, (w.y + e.y) / 2); }
    point neast() const {
        return point(w.x < e.x ? e.x : w.x, e.y < w.y ? w.y : e.y);
    }
    point seast() const {
        return point(w.x < e.x ? e.x : w.x, e.y < w.y ? e.y : w.y);
    }
    point nwest() const {
        return point(w.x < e.x ? w.x : e.x, e.y < w.y ? w.y : e.y);
    }
    point swest() const {
        return point(w.x < e.x ? w.x : e.x, e.y < w.y ? e.y : w.y);
    }
    void move(int a, int b) {
        try {
            w.x += a;
            w.y += b;
            if (!on_screen(w.x, w.y)) {throw CantBeMoved(w.x, w.y);}
            e.x += a;
            e.y += b;
            if (!on_screen(e.x, e.y)) {throw CantBeMoved(e.x, e.y);}
        } catch (CantBeMoved& ex) {
            ex.show();
        }

    }
    void draw() {
        try {
            put_line(w, e);
        } catch (OutOfScreen& ex) {
            ex.show();

        }
    }
    void resize(double d) // Изменение длины линии в (d) раз
    {
        e.x = w.x + (e.x - w.x) * d;
        e.y = w.y + (e.y - w.y) * d;
    }
};
void up(shape &p, const shape &q) // поместить фигуру p над фигурой q
{ // Это ОБЫЧНАЯ функция, не член класса! Динамическое связывание!!
    point n = q.north();
    point s = p.south();

    try {
        p.move(n.x - s.x, n.y - s.y + 1);
    } catch (CantBeMoved& ex) {
        ex.show();
    }
}

class rectangle : public rotatable { // ==== Прямоугольник ====
                                     /* nw ------ n ------ ne
                                        |		            |
                                        |		            |
                                        w	   c            e
                                        |		            |
                                        |		            |
                                        sw ------- s ----- se */
  protected:
    point sw, ne;

  public:
    rectangle(point a, point b) : sw(a), ne(b) {}
    point north() const { return point((sw.x + ne.x) / 2, ne.y); }
    point south() const { return point((sw.x + ne.x) / 2, sw.y); }
    point east() const { return point(ne.x, (sw.y + ne.y) / 2); }
    point west() const { return point(sw.x, (sw.y + ne.y) / 2); }
    point neast() const { return ne; }
    point seast() const { return point(ne.x, sw.y); }
    point nwest() const { return point(sw.x, ne.y); }
    point swest() const { return sw; }
    void rotate_right() // Поворот вправо относительно se
    {

        int w = ne.x - sw.x, h = ne.y - sw.y; // (учитывается масштаб по осям)
        sw.x = ne.x - h * 2;
        ne.y = sw.y + w / 2;
    }
    void rotate_left() // Поворот влево относительно sw
    {
        int w = ne.x - sw.x, h = ne.y - sw.y;
        ne.x = sw.x + h * 2;
        ne.y = sw.y + w / 2;
    }
    void move(int a, int b) {
        try {
            sw.x += a;
            sw.y += b;
            ne.x += a;
            ne.y += b;
            if (!on_screen(ne.x, ne.y)) {throw CantBeMoved(ne.x, ne.y);}
            else if (!on_screen(sw.x, sw.y)) {throw CantBeMoved(sw.x, sw.y);}
        } catch (CantBeMoved& ex) {
            ex.show();
        }
    }
    void resize(double d) {
        ne.x = sw.x + (ne.x - sw.x) * d;
        ne.y = sw.y + (ne.y - sw.y) * d;
    }
    void draw() {
        try {
            put_line(nwest(), ne);

            put_line(ne, seast());
            put_line(seast(), sw);
            put_line(sw, nwest());
        } catch (OutOfScreen& ex) {
            ex.show();

        }

    }
};
