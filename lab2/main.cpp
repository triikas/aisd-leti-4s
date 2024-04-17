#include "figure.h"
#include <locale>
// ПРИМЕР ДОБАВКИ: дополнительная функция присоединения…
void down(shape &p, const shape &q) {
    point n = q.south();
    point s = p.north();
    p.move(n.x - s.x, n.y - s.y - 1);
}

class h_circle : public rectangle, public reflectable {
    bool reflected;

public:
    h_circle(point a, point b, bool r = true) : rectangle(a, b), reflected(r) {}
    void draw();
    void flip_horisontally(){}; // Отразить горизонтально (пустая функция)
    void flip_vertically() { reflected = !reflected; }; // Отразить вертикально
};
void h_circle ::draw() // Алгоритм Брезенхэма для окружностей
{ // (выдаются два сектора, указываемые значением reflected)
    int x0 = (sw.x + ne.x) / 2, y0 = reflected ? sw.y : ne.y;
    int radius = (ne.x - sw.x) / 2;
    int x = 0, y = radius, delta = 2 - 2 * radius, error = 0;
    while (y >= 0) { // Цикл рисования
        if (reflected) {
            put_point(x0 + x, y0 + y * 0.7);
            put_point(x0 - x, y0 + y * 0.7);
        } else {
            put_point(x0 + x, y0 - y * 0.7);
            put_point(x0 - x, y0 - y * 0.7);
        }
        error = 2 * (delta + y) - 1;
        if (delta < 0 && error <= 0) {
            ++x;
            delta += 2 * x + 1;
            continue;
        }
        error = 2 * (delta - x) - 1;
        if (delta > 0 && error > 0) {
            --y;
            delta += 1 - 2 * y;
            continue;
        }
        ++x;
        delta += 2 * (x - y);
        --y;
    }
}

class MyShape : public rotatable, public reflectable {
private:
    point sw, nw, se, ne;

public:
    MyShape(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4)
            : sw(x1, y1), nw(x2, y2), se(x3, y3), ne(x4, y4){};
    point nwest() const { return nw; };
    point swest() const { return sw; };
    point neast() const { return ne; };
    point seast() const { return se; };
    point north() const { return point((sw.x + se.x) / 2, nw.y); };
    point south() const { return point((sw.x + se.x) / 2, se.y); };
    point east() const {
        return point((se.x + nwest().x) / 2, (nw.y + se.y) / 2);
    };
    point west() const { return point((sw.x + nw.x) / 2, (nw.y + se.y) / 2); };
    void draw() {
        put_line(sw, nw);
        put_line(nw, ne);
        put_line(ne, se);
        put_line(se, sw);
        if (ne.y > nw.y && ne.x < se.x) {
            put_line(sw, point(ne.x, se.y));
            put_line(se, point(nw.x, sw.y));
        } else if (ne.y > nw.y && ne.x > se.x) {
            put_line(sw, point(ne.x, se.y));
            put_line(se, point(nw.x, sw.y));
        } else {
            put_line(sw, point(se.x, ne.y));
            put_line(se, point(sw.x, nw.y));
        }
    }
    void move(int a, int b) {
        sw.x += a;
        sw.y += b;
        nw.x += a;
        nw.y += b;
        se.x += a;
        se.y += b;
        ne.x += a;
        ne.y += b;
    }
    void rotate_right() {
        point center = point(south().y, east().x);
        point p1 = sw, p2 = nw, p3 = se, p4 = ne;
        nw.y = center.y - (p2.x - center.x);
        nw.x = center.x - (center.y - p2.y);
        ne.y = center.y - (p4.x - center.x);
        ne.x = center.x + (p4.y - center.y);
        sw.y = center.y + (center.x - p1.x);
        sw.x = center.x - (center.y - p1.y);
        se.y = center.y + (center.x - p3.x);
        se.x = center.x + (p3.y - center.y);
    }
    void rotate_left() {
        point center = point(south().x, east().y);
        point p1 = sw, p2 = nw, p3 = se, p4 = ne;
        nw.x = center.x - (p2.y - center.y);
        nw.y = center.y - (center.x - p2.x);
        ne.x = center.x - (p4.y - center.y);
        ne.y = center.y + (p4.x - center.x);
        sw.x = center.x + (center.y - p1.y);
        sw.y = center.y - (center.x - p1.x);
        se.x = center.x + (center.y - p3.y);
        se.y = center.y + (p3.x - center.x);
    }
    void flip_horisontally() {
        point center = point(south().x, east().y);
        sw = point(center.x + (center.x - sw.x), sw.y);
        nw = point(center.x + (center.x - nw.x), nw.y);
        se = point(center.x - (se.x - center.x), se.y);
        ne = point(center.x - (ne.x - center.x), ne.y);
    }
    void flip_vertically() {
        point center = point(south().x, east().y);
        sw = point(sw.x, center.y + (center.y - sw.y));
        nw = point(nw.x, center.y + (center.y - nw.y));
        se = point(se.x, center.y - (se.y - center.y));
        ne = point(ne.x, center.y - (ne.y - center.y));
    }
    void resize(double d) {
        sw.x *= d;
        sw.y *= d;
        nw.x *= d;
        nw.y *= d;
        se.x *= d;
        se.y *= d;
        ne.x *= d;
        ne.y *= d;
    }
};
// Cборная пользовательская фигура – физиономия
class myshape : public rectangle { // Моя фигура ЯВЛЯЕТСЯ
    int w, h;                      //        прямоугольником
    line l_eye; // левый глаз – моя фигура СОДЕРЖИТ линию
    line r_eye; // правый глаз
    line mouth; // рот
public:
    myshape(point, point);
    void draw();
    void move(int, int);
    void resize(int) {}
};
myshape ::myshape(point a, point b)
        : rectangle(a, b), // Инициализация базового класса
          w(neast().x - swest().x + 1), // Инициализация данных
          h(neast().y - swest().y + 1), // – строго в порядке объявления!
          l_eye(point(swest().x + 2, swest().y + h * 3 / 4), 2),
          r_eye(point(swest().x + w - 4, swest().y + h * 3 / 4), 2),
          mouth(point(swest().x + 2, swest().y + h / 4), w - 4) {}
void myshape ::draw() {
    rectangle ::draw(); // Контур лица (глаза и нос рисуются сами!)
    int a = (swest().x + neast().x) / 2;
    int b = (swest().y + neast().y) / 2;
    put_point(point(a, b)); // Нос – существует только на рисунке!
}
void myshape ::move(int a, int b) {
    rectangle ::move(a, b);
    l_eye.move(a, b);
    r_eye.move(a, b);
    mouth.move(a, b);
}
int main() {
    setlocale(LC_ALL, "Rus");
    screen_init();

    //== 1. Объявление набора фигур ==

    rectangle hat(point(0, 0), point(14, 5));
    line brim(point(0, 15), 17);
    myshape face(point(15, 10), point(27, 18));
    h_circle beard(point(40, 10), point(50, 20));
    MyShape left_ear(0, 0, 6, 24, 24, 0, 18, 24);
    MyShape right_ear(0, 0, 6, 24, 24, 0, 18, 24);
    MyShape symbol(0, 0, 2, 7, 10, 0, 8, 7);
    shape_refresh();
    std::cout << "=== Generated... ===\n";
    std::cin.get(); // Смотреть исходный набор

    //== 2. Подготовка к сборке ==

    hat.rotate_right();
    brim.resize(2.0);
    face.resize(2.0);
    beard.flip_vertically();
    left_ear.rotate_left();
    left_ear.resize(0.2);
    right_ear.rotate_left();
    right_ear.resize(0.2);
    right_ear.flip_horisontally();
    shape_refresh();
    std::cout << "=== Prepared... ===\n";
    std::cin.get(); // Смотреть результат поворотов/отражений

    //== 3. Сборка изображения ==

    up(brim, face);
    up(hat, brim);
    left_ear.move(11, 12);
    right_ear.move(23, 12);
    symbol.move(16, 20);
    down(beard, face);
    shape_refresh();
    std::cout << "=== Ready! ===\n";
    std::cin.get(); // Смотреть результат
    screen_destroy();
    return 0;
}