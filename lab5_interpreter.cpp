#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <regex>
#include <algorithm>

using namespace std;

class Context {
private:
    string text;

public:
    Context(string inputText) {
        text = inputText;
    }

    string getText() const {
        return text;
    }

    void setText(string newText) {
        text = newText;
    }

    void printText() const {
        cout << text << endl;
    }
};

class Expression {
public:
    virtual ~Expression() {}
    virtual void interpret(Context& context) = 0;
    virtual string getName() const = 0;
};

class MultipleSpacesExpression : public Expression {
public:
    void interpret(Context& context) override {
        string text = context.getText();
        text = regex_replace(text, regex(" {2,}"), " ");
        context.setText(text);
    }

    string getName() const override {
        return "Удаление множественных пробелов";
    }
};

class DashExpression : public Expression {
public:
    void interpret(Context& context) override {
        string text = context.getText();

        text = regex_replace(text, regex(" ?- ?"), " — ");
        text = regex_replace(text, regex(" {2,}"), " ");

        context.setText(text);
    }

    string getName() const override {
        return "Замена дефиса на тире";
    }
};

class QuotesExpression : public Expression {
public:
    void interpret(Context& context) override {
        string text = context.getText();
        string result;
        bool opened = false;

        for (size_t i = 0; i < text.size(); i++) {
            if (text[i] == '"') {
                if (!opened) {
                    result += "«";
                    opened = true;
                }
                else {
                    result += "»";
                    opened = false;
                }
            }
            else {
                result += text[i];
            }
        }

        context.setText(result);
    }

    string getName() const override {
        return "Замена кавычек";
    }
};

class TabsExpression : public Expression {
public:
    void interpret(Context& context) override {
        string text = context.getText();
        replace(text.begin(), text.end(), '\t', ' ');
        text = regex_replace(text, regex(" {2,}"), " ");
        context.setText(text);
    }

    string getName() const override {
        return "Исправление табуляторов";
    }
};

class BracketsAndPunctuationExpression : public Expression {
public:
    void interpret(Context& context) override {
        string text = context.getText();

        // Убираем лишний пробел после открывающей скобки
        text = regex_replace(text, regex("\\( +"), "(");

        // Убираем лишний пробел перед закрывающей скобкой
        text = regex_replace(text, regex(" +\\)"), ")");

        // Убираем лишний пробел перед запятой и точкой
        text = regex_replace(text, regex(" +,"), ",");
        text = regex_replace(text, regex(" +\\."), ".");

        // После запятой и точки должен быть один пробел, если дальше идет символ
        text = regex_replace(text, regex(",([^ \\n])"), ", $1");
        text = regex_replace(text, regex("\\.([^ \\n])"), ". $1");

        context.setText(text);
    }

    string getName() const override {
        return "Исправление скобок и пунктуации";
    }
};

class LineBreakExpression : public Expression {
public:
    void interpret(Context& context) override {
        string text = context.getText();

        text = regex_replace(text, regex("\\n{3,}"), "\n\n");
        context.setText(text);
    }

    string getName() const override {
        return "Удаление лишних переводов строки";
    }
};

class TextInterpreter {
private:
    vector<shared_ptr<Expression>> expressions;

public:
    void addExpression(shared_ptr<Expression> expression) {
        expressions.push_back(expression);
    }

    void interpret(Context& context) {
        cout << "Запуск интерпретатора текста" << endl;

        for (size_t i = 0; i < expressions.size(); i++) {
            cout << i + 1 << ") " << expressions[i]->getName() << endl;
            expressions[i]->interpret(context);
        }

        cout << "Интерпретация завершена" << endl;
    }
};

class TextEditor {
private:
    Context context;
    TextInterpreter interpreter;

public:
    TextEditor(string text) : context(text) {
        interpreter.addExpression(make_shared<TabsExpression>());
        interpreter.addExpression(make_shared<LineBreakExpression>());
        interpreter.addExpression(make_shared<QuotesExpression>());
        interpreter.addExpression(make_shared<DashExpression>());
        interpreter.addExpression(make_shared<BracketsAndPunctuationExpression>());
        interpreter.addExpression(make_shared<MultipleSpacesExpression>());
    }

    void showOriginalText() {
        cout << "Исходный текст:" << endl;
        context.printText();
    }

    void correctText() {
        interpreter.interpret(context);
    }

    void showCorrectedText() {
        cout << "Исправленный текст:" << endl;
        context.printText();
    }
};

int main() {
    string text = "Это   пример    текста  -  он содержит \"ошибки\" .\n\n\n"
                  "В тексте есть\tтабуляция , лишние пробелы ( внутри скобок ) .\n"
                  "Также используется дефис - вместо тире.";

    TextEditor editor(text);

    editor.showOriginalText();
    cout << endl;

    editor.correctText();
    cout << endl;

    editor.showCorrectedText();

    return 0;
}
