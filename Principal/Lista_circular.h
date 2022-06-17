#pragma once
#include<vector>
#include<iostream>
#include <string>

class ListaCircular {
private:
    std::vector <std::string> container;
    int max;
    int counter = 0;
    int pointer;
public:
    ListaCircular(int size) {
        this->max = size;
        this->pointer = max;
    }

    bool push(std::string data) {
        if ((this->pointer > 0) && (this->counter < this->max)) { //Lista com espa?o a serem alocados
            this->container.push_back(data);
            this->counter++;
            this->pointer--;
            return true;
        }
        else if (this->pointer > 0) { //Lista ja contem N itens que ser?o e podem ser sobrescritos
            this->container[(this->counter) % (this->max)] = data;
            this->counter++;
            this->pointer--;
            return true;
        }
        else { //Lista cheia com N itens, e como nenhum foi lido n?o pode se adicionar mais nada
            return false;
        }
    }

    int getCounter() {
        return this->counter;
    }

    int getPointer() {
        return this->pointer;
    }

    std::string getItem(int i) {
        int position = (i) % (this->max);
        return this->container[position];
    }

    std::string readItem(int i) { //Fun??o de leitura, a ser usada para "remover" item da lista

        this->pointer++; //Abre vaga na lista
        int position = (i) % (this->max);
        return this->container[position];

    }

    void print() {

        std::cout << std::endl;
        for (int i = 0; i < this->container.size(); i++)
            std::cout << this->container[i] << std::endl;

    }
};
