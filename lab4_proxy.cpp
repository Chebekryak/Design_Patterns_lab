#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <cmath>
#include <optional>

using namespace std;

class IImage {
public:
    virtual void draw(sf::RenderWindow& window) = 0;
    virtual void setPosition(float x, float y) = 0;
    virtual sf::Vector2f getPosition() const = 0;
    virtual sf::Vector2f getSize() const = 0;
    virtual bool contains(float x, float y) const = 0;
    virtual bool isLoaded() const = 0;
    virtual ~IImage() {}
};

class RealImage : public IImage {
private:
    string fileName;
    sf::Texture texture;
    sf::Sprite sprite;
    sf::Vector2f position;
    sf::Vector2f size;
    bool loaded;

public:
    RealImage(const string& fileName, sf::Vector2f position, sf::Vector2f size)
        : fileName(fileName), sprite(texture), position(position), size(size), loaded(false) {
        cout << "RealImage: пытаюсь загрузить файл " << fileName << endl;

        if (!texture.loadFromFile(fileName)) {
            cout << "RealImage: ошибка загрузки. Проверь, что " << fileName
                 << " лежит рядом с exe-файлом." << endl;
            return;
        }

        loaded = true;
        sprite.setTexture(texture, true);
        sprite.setPosition(position);

        sf::Vector2u textureSize = texture.getSize();
        if (textureSize.x != 0 && textureSize.y != 0) {
            sprite.setScale(sf::Vector2f(size.x / textureSize.x, size.y / textureSize.y));
        }

        cout << "RealImage: изображение успешно загружено" << endl;
    }

    void draw(sf::RenderWindow& window) override {
        if (loaded) {
            window.draw(sprite);
        }
    }

    void setPosition(float x, float y) override {
        position = sf::Vector2f(x, y);
        sprite.setPosition(position);
    }

    sf::Vector2f getPosition() const override { return position; }
    sf::Vector2f getSize() const override { return size; }

    bool contains(float x, float y) const override {
        return x >= position.x && x <= position.x + size.x &&
               y >= position.y && y <= position.y + size.y;
    }

    bool isLoaded() const override { return loaded; }
};

class ImageProxy : public IImage {
private:
    string fileName;
    sf::Vector2f position;
    sf::Vector2f size;
    RealImage* realImage;
    sf::RectangleShape box;
    sf::RectangleShape corner;

public:
    ImageProxy(const string& fileName, float width, float height) {
        this->fileName = fileName;
        position = sf::Vector2f(120, 120);
        size = sf::Vector2f(width, height);
        realImage = nullptr;

        box.setSize(size);
        box.setPosition(position);
        box.setFillColor(sf::Color(210, 225, 255));
        box.setOutlineColor(sf::Color::Black);
        box.setOutlineThickness(4);

        corner.setSize(sf::Vector2f(40, 40));
        corner.setFillColor(sf::Color(80, 120, 240));
        corner.setPosition(position);

        cout << "ImageProxy: создан видимый синий бокс " << width << " x " << height << endl;
        cout << "ImageProxy: файл TestImage.bmp еще НЕ загружен" << endl;
    }

    ~ImageProxy() { delete realImage; }

    void loadRealImage() {
        if (realImage == nullptr) {
            realImage = new RealImage(fileName, position, size);
            if (realImage->isLoaded()) {
                cout << "ImageProxy: Proxy создал RealImage только после двойного клика" << endl;
            }
        } else {
            cout << "ImageProxy: RealImage уже создан" << endl;
        }
    }

    void draw(sf::RenderWindow& window) override {
        if (realImage != nullptr && realImage->isLoaded()) {
            realImage->draw(window);
        } else {
            window.draw(box);
            window.draw(corner);
        }
    }

    void setPosition(float x, float y) override {
        position = sf::Vector2f(x, y);
        box.setPosition(position);
        corner.setPosition(position);
        if (realImage != nullptr) {
            realImage->setPosition(x, y);
        }
    }

    sf::Vector2f getPosition() const override { return position; }
    sf::Vector2f getSize() const override { return size; }

    bool contains(float x, float y) const override {
        return x >= position.x && x <= position.x + size.x &&
               y >= position.y && y <= position.y + size.y;
    }

    bool isLoaded() const override {
        return realImage != nullptr && realImage->isLoaded();
    }
};

class GraphicEditor {
private:
    sf::RenderWindow window;
    ImageProxy image;
    bool dragging;
    sf::Vector2f dragOffset;
    sf::Clock doubleClickClock;
    sf::Vector2f lastRightClickPosition;
    bool hasRightClick;

public:
    GraphicEditor()
        : window(sf::VideoMode(sf::Vector2u(1000, 700)), "Lab 4 - Proxy Graphic Editor SFML 3"),
          image("TestImage.bmp", 320, 220) {
        dragging = false;
        dragOffset = sf::Vector2f(0, 0);
        hasRightClick = false;
        window.setFramerateLimit(60);
        cout << "GraphicEditor: окно создано. Если ты видишь синий бокс, программа работает." << endl;
    }

    void run() {
        while (window.isOpen()) {
            handleEvents();
            draw();
        }
    }

private:
    void handleEvents() {
        while (const optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::Escape) {
                    window.close();
                }
            }

            if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                float mouseX = static_cast<float>(mousePressed->position.x);
                float mouseY = static_cast<float>(mousePressed->position.y);

                if (mousePressed->button == sf::Mouse::Button::Left) {
                    if (image.contains(mouseX, mouseY)) {
                        dragging = true;
                        sf::Vector2f position = image.getPosition();
                        dragOffset = sf::Vector2f(mouseX - position.x, mouseY - position.y);
                        cout << "Editor: начато перемещение бокса" << endl;
                    }
                }

                if (mousePressed->button == sf::Mouse::Button::Right) {
                    checkRightDoubleClick(mouseX, mouseY);
                }
            }

            if (const auto* mouseReleased = event->getIf<sf::Event::MouseButtonReleased>()) {
                if (mouseReleased->button == sf::Mouse::Button::Left) {
                    dragging = false;
                }
            }

            if (const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>()) {
                if (dragging) {
                    float mouseX = static_cast<float>(mouseMoved->position.x);
                    float mouseY = static_cast<float>(mouseMoved->position.y);
                    moveImage(mouseX - dragOffset.x, mouseY - dragOffset.y);
                }
            }
        }
    }

    void checkRightDoubleClick(float mouseX, float mouseY) {
        float time = doubleClickClock.getElapsedTime().asSeconds();
        float distance = 1000.0f;

        if (hasRightClick) {
            float dx = mouseX - lastRightClickPosition.x;
            float dy = mouseY - lastRightClickPosition.y;
            distance = sqrt(dx * dx + dy * dy);
        }

        if (hasRightClick && time < 0.45f && distance < 12.0f) {
            if (image.contains(mouseX, mouseY)) {
                cout << "Editor: двойной правый клик внутри бокса" << endl;
                image.loadRealImage();
                window.setTitle("Lab 4 - image loaded through Proxy");
            } else {
                cout << "Editor: двойной правый клик вне бокса" << endl;
            }
            hasRightClick = false;
        } else {
            hasRightClick = true;
            lastRightClickPosition = sf::Vector2f(mouseX, mouseY);
            doubleClickClock.restart();
            cout << "Editor: первый правый клик" << endl;
        }
    }

    void moveImage(float x, float y) {
        sf::Vector2f size = image.getSize();
        sf::Vector2u windowSize = window.getSize();

        if (x < 0) x = 0;
        if (y < 0) y = 0;
        if (x + size.x > windowSize.x) x = static_cast<float>(windowSize.x) - size.x;
        if (y + size.y > windowSize.y) y = static_cast<float>(windowSize.y) - size.y;

        image.setPosition(x, y);
    }

    void draw() {
        window.clear(sf::Color(245, 245, 245));
        image.draw(window);
        window.display();
    }
};

int main() {
    setlocale(LC_ALL, "Russian");

    cout << "Лабораторная работа 4. Паттерн Proxy" << endl;
    cout << "Должно открыться окно 1000x700 с синим боксом." << endl;
    cout << "ЛКМ по боксу — перемещение." << endl;
    cout << "Двойной ПКМ внутри бокса — загрузка TestImage.bmp." << endl;
    cout << "Esc — выход." << endl;

    GraphicEditor editor;
    editor.run();

    return 0;
}
