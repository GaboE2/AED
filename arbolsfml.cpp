#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <cmath>
#include <string>
#include <SFML/Graphics.hpp>

using namespace std;

//Arbol
struct nodo {
    int valor;
    nodo* izq;
    nodo* der;
};

nodo* CreaArbol(int* ini, int* fin) {
    int n = (int)(fin - ini);
    if (n == 0) return nullptr;

    static nodo arbol[64];
    nodo* cola[64];
    int frente = 0, final = 0;

    for (int i = 0; i < n; i++) {
        arbol[i].valor = ini[i];
        arbol[i].izq = nullptr;
        arbol[i].der = nullptr;
        cola[final++] = &arbol[i];
    }

    int indice_actual = n;

    while (frente < final - 1) {
        nodo* izq = cola[frente++];
        nodo* der = cola[frente++];

        arbol[indice_actual].valor = izq->valor + der->valor;
        arbol[indice_actual].izq = izq;
        arbol[indice_actual].der = der;

        cola[final++] = &arbol[indice_actual];
        indice_actual++;
    }

    return cola[frente]; // raíz
}

// Arbol Templates

template <class NodePtr, class GetL, class GetR, class GetLabel>
class TreeDrawer {
public:
    TreeDrawer(NodePtr root,
        GetL getLeft,
        GetR getRight,
        GetLabel getLabel,
        float nodeRadius = 22.f,
        float levelGap = 110.f,
        float marginX = 40.f,
        float marginY = 40.f)
        : root(root),
        getLeft(getLeft),
        getRight(getRight),
        getLabel(getLabel),
        R(nodeRadius),
        gapY(levelGap),
        mx(marginX),
        my(marginY) {
    }

    // Genera todos los drawables (líneas, círculos, textos) según el tamaño de ventana
    void build(float windowW, float windowH, sf::Font& font) {
        (void)windowH; 
		// limpiar buffer
        lines.clear();
        circles.clear();
        labels.clear();
        pos.clear();
        order.clear();
        level.clear();

        // 1) Recorrido para calcular niveles e in-order
        int maxLevel = 0;
        inorderAndLevel(root, 0, maxLevel);

        if (order.empty()) return;

        // 2) Espaciado por columnas con base al 
        const int cols = (int)order.size();
        const float usableW = max(200.f, windowW - 2 * mx);
        const float colStep = (cols > 1) ? (usableW / (cols - 1)) : 0.f;

        // Altura estimada según niveles
        // gapY y margen superior.

        // Asignar posiciones
        for (int i = 0; i < (int)order.size(); ++i) {
            auto p = order[i];
            float x = mx + i * colStep;
            float y = my + (float)level[p] * gapY;
            pos[p] = sf::Vector2f(x, y);
        }

        // Lineas pADRE Hijo
        for (auto p : order) {
            if (!p) continue;
            auto L = getLeft(p);
            auto Rn = getRight(p);
            if (L) addEdge(p, L);
            if (Rn) addEdge(p, Rn);
        }

        // 6) Crear nodos 
        for (auto p : order) {
            if (!p) continue;
            auto P = pos[p];

            sf::CircleShape c(R);
            c.setFillColor(sf::Color(240, 240, 240));
            c.setOutlineColor(sf::Color(60, 60, 60));
            c.setOutlineThickness(2.f);
            c.setOrigin(R, R);
            c.setPosition(P);
            circles.push_back(c);

            sf::Text t;
            t.setFont(font);
            t.setString(getLabel(p));
            t.setCharacterSize(18);
            t.setFillColor(sf::Color::Black);

            // Centrar texto en el círculo:
            auto bounds = t.getLocalBounds();
            t.setOrigin(bounds.left + bounds.width / 2.f,
                bounds.top + bounds.height / 2.f);
            t.setPosition(P);
            labels.push_back(t);
        }
    }

    void draw(sf::RenderWindow& win) {
        // Dibujar primero líneas, luego nodos, luego textos
        for (auto& ln : lines)      win.draw(ln);
        for (auto& c : circles)     win.draw(c);
        for (auto& t : labels)      win.draw(t);
    }

    // Profundidad máxima  ventana
    int maxDepth() const {
        int m = 0;
        for (auto& kv : level) m = max(m, kv.second);
        return m;
    }

private:
    NodePtr root;
    GetL getLeft;
    GetR getRight;
    GetLabel getLabel;

    float R, gapY, mx, my;

    // almacenamiento
    vector<sf::VertexArray> lines;
    vector<sf::CircleShape> circles;
    vector<sf::Text>        labels;

    //puntero como void* para que funcione en unordered_map
    struct PtrHash {
        size_t operator()(const void* p) const noexcept {
            return std::hash<uintptr_t>()(reinterpret_cast<uintptr_t>(p));
        }
    };
    struct PtrEq {
        bool operator()(const void* a, const void* b) const noexcept {
            return a == b;
        }
    };

    unordered_map<const void*, sf::Vector2f, PtrHash, PtrEq> pos;
    unordered_map<const void*, int, PtrHash, PtrEq>          level;
    vector<NodePtr> order;

    void inorderAndLevel(NodePtr p, int lvl, int& maxLevel) {
        if (!p) return;
        maxLevel = max(maxLevel, lvl);
        inorderAndLevel(getLeft(p), lvl + 1, maxLevel);
        order.push_back(p);
        level[p] = lvl;
        inorderAndLevel(getRight(p), lvl + 1, maxLevel);
    }

    void addEdge(NodePtr parent, NodePtr child) {
        auto P = pos[parent];
        auto C = pos[child];
        sf::VertexArray va(sf::Lines, 2);
        va[0].position = P;
        va[0].color = sf::Color(80, 80, 80);
        va[1].position = C;
        va[1].color = sf::Color(80, 80, 80);
        lines.push_back(va);
    }
};

int main() {
    
    int arr[16] = { 2,4,7,9,10,15,3,25,38,74,1,5,10,13,11,35 };
    int n = sizeof(arr) / sizeof(arr[0]);
    nodo* raiz = CreaArbol(arr, arr + n);

    // Ventana
    const unsigned W = 1280;
    const unsigned H = 800;
    sf::RenderWindow window(sf::VideoMode(W, H), "Arbol con SFML (generico)");
    window.setFramerateLimit(60);

    // Fuente
    sf::Font font;
    if (!font.loadFromFile("ArialCEItalic.ttf")) {
        std::cerr << "no hay fuente\n";
        return 1;
    }

    // lambda para cualquier arbol
    auto getLeft = [](nodo* p) -> nodo* { return p ? p->izq : nullptr; };
    auto getRight = [](nodo* p) -> nodo* { return p ? p->der : nullptr; };
    auto getLabel = [](nodo* p) -> string { return to_string(p->valor);  };

	// dibujador de arbol   
    TreeDrawer<nodo*, decltype(getLeft), decltype(getRight), decltype(getLabel)>
        drawer(raiz, getLeft, getRight, getLabel,
            /*radio*/22.f, /*gapY*/110.f, /*margenX*/60.f, /*margenY*/50.f);

    // Construye los drawables
    drawer.build((float)W, (float)H, font);

    // Bucle
    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) window.close();
            if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Escape)
                window.close();
        }
        window.clear(sf::Color(250, 250, 255));
        drawer.draw(window);
        window.display();
    }
    return 0;
}
