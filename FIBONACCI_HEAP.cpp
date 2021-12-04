#include <iomanip>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <fstream>
using namespace std;

template <class T>
class Nodo {
    public:
        T m_Dato;
        int grado;
        Nodo<T> *left;
        Nodo<T> *right;
        Nodo<T> *m_Son;
        Nodo<T> *padre;
        bool Color; 

        Nodo(T value):m_Dato(value), grado(0), Color(false), left(nullptr),right(nullptr),m_Son(nullptr),padre(nullptr){
            m_Dato    = value;
            grado = 0;
            Color = false;
            left   = this;
            right  = this;
            padre = nullptr;
            m_Son  = nullptr;
        }
};

template <class T>
class FibonacciHeap {
    private:
        int m_DatoNum;   
        int maxgrado;  
        Nodo<T> *min; 
        Nodo<T> **cons;  

    public:
        FibonacciHeap()
        {
            m_DatoNum = 0;
            maxgrado = 0;
            min = nullptr;
            cons = nullptr;
        }

        ~FibonacciHeap(){}

        void insert(T m_Dato)
        {
            Nodo<T> *node;

            node = new Nodo<T>(m_Dato);
            if (node == nullptr)
                return ;

            insert(node);
        }

        void removeMin()
        {
            if (min==nullptr)
                return ;

            Nodo<T> *m_Son = nullptr;
            Nodo<T> *m = min;
            while (m->m_Son != nullptr){
                m_Son = m->m_Son;
                removeNode(m_Son);
                if (m_Son->right == m_Son)
                    m->m_Son = nullptr;
                else
                    m->m_Son = m_Son->right;

                addNode(m_Son, min);
                m_Son->padre = nullptr;
            }
            removeNode(m);
            if (m->right == m)
                min = nullptr;
            else{
                min = m->right;
                consolidate();
            }
            m_DatoNum--;

            delete m;
        }

        void combine(FibonacciHeap<T> *other)
        {
            if (other==nullptr)
                return ;

            if(other->maxgrado > this->maxgrado)
                swap(*this, *other);

            if((this->min) == nullptr){ 
                this->min = other->min;
                this->m_DatoNum = other->m_DatoNum;
                free(other->cons);
                delete other;
            }
            else if((other->min) == nullptr) {    
                free(other->cons);
                delete other;
            }                   
            else{
                catList(this->min, other->min);

                if (this->min->m_Dato > other->min->m_Dato)
                    this->min = other->min;
                this->m_DatoNum += other->m_DatoNum;
                free(other->cons);
                delete other;
            }
        }

        bool minimum(T *pm_Dato)
        {
            if (min==nullptr || pm_Dato==nullptr)
                return false;
            *pm_Dato = min->m_Dato;
            return true;
        }

        void update(T oldm_Dato, T newm_Dato)
        {
            Nodo<T> *node;
            node = search(oldm_Dato);
            if (node!=nullptr)
                update(node, newm_Dato);
        }

        void remove(T m_Dato)
        {
            Nodo<T> *node;
            if (min==nullptr)
                return ;
            node = search(m_Dato);
            if (node==nullptr)
                return ;
            remove(node);
        }

        

        bool contains(T m_Dato)
        {
            return search(m_Dato)!=nullptr ? true: false;
        }

        void print(ostream& out)
        {
            int i=0;
            Nodo<T> *p;

            if (min==nullptr)
                return ;
            p = min;
            do {
                i++;
                out  << p->m_Dato << ";" << endl;

                print(p->m_Son, p, 1, out);
                p = p->right;
            } while (p != min);
            cout << endl;
        }


        void destroy()
        {
            destroyNode(min);
            free(cons);
        }


    private:
        void removeNode(Nodo<T> *node)
        {
            node->left->right = node->right;
            node->right->left = node->left;
        }

        void addNode(Nodo<T> *node, Nodo<T> *root)
        {
            node->left        = root->left;
            root->left->right = node;
            node->right       = root;
            root->left        = node;
        }

        void catList(Nodo<T> *a, Nodo<T> *b)
        {
            Nodo<T> *tmp;
            tmp            = a->right;
            a->right       = b->right;
            b->right->left = a;
            b->right       = tmp;
            tmp->left      = b;
        }

        void insert(Nodo<T> *node)
        {
            if (m_DatoNum == 0)
                min = node;
            else{
                addNode(node, min);
                if (node->m_Dato < min->m_Dato)
                    min = node;
            }
            m_DatoNum++;
        }

        Nodo<T>* extractMin()
        {
            Nodo<T> *p = min;

            if (p == p->right)
                min = nullptr;
            else{
                removeNode(p);
                min = p->right;
            }
            p->left = p->right = p;
            return p;
        }

        void link(Nodo<T>* node, Nodo<T>* root)
        {
            removeNode(node);
            if (root->m_Son == nullptr)
                root->m_Son = node;
            else
                addNode(node, root->m_Son);

            node->padre = root;
            root->grado++;
            node->Color = false;
        }

        void makeCons()
        {
            int old = maxgrado;
            maxgrado = (log(m_DatoNum)/log(2.0)) + 1;
            if (old >= maxgrado)
                return ;
            cons = (Nodo<T> **)realloc(cons,
                    sizeof(FibonacciHeap<T> *) * (maxgrado + 1));
        }

        void consolidate()
        {
            int i, d, D;
            Nodo<T> *x, *y, *tmp;

            makeCons();
            D = maxgrado + 1;

            for (i = 0; i < D; i++)
                cons[i] = nullptr;
            while (min != nullptr){
                x = extractMin();     
                d = x->grado;
                while (cons[d] != nullptr){
                    y = cons[d];
                    if (x->m_Dato > y->m_Dato)
                        swap(x, y);

                    link(y, x);
                    cons[d] = nullptr;
                    d++;
                }
                cons[d] = x;
            }
            min = nullptr;
            for (i=0; i<D; i++){
                if (cons[i] != nullptr){
                    if (min == nullptr)
                        min = cons[i];
                    else{
                        addNode(cons[i], min);
                        if ((cons[i])->m_Dato < min->m_Dato)
                            min = cons[i];
                    }
                }
            }
        }

        void renewgrado(Nodo<T> *padre, int grado)
        {
            padre->grado -= grado;
            if (padre-> padre != nullptr)
                renewgrado(padre->padre, grado);
        }

        void cut(Nodo<T> *node, Nodo<T> *padre)
        {
            removeNode(node);
            renewgrado(padre, node->grado);
            if (node == node->right)
                padre->m_Son = nullptr;
            else
                padre->m_Son = node->right;
            node->padre = nullptr;
            node->left = node->right = node;
            node->Color = false;
            addNode(node, min);
        }

        void cascadingCut(Nodo<T> *node)
        {
            Nodo<T> *padre = node->padre;
            if (padre != nullptr){
                if (node->Color == false)
                    node->Color = true;
                else{
                    cut(node, padre);
                    cascadingCut(padre);
                }
            }
        }

        void decrease(Nodo<T> *node, T m_Dato)
        {
            Nodo<T> *padre;
            if (min==nullptr ||node==nullptr)
                return ;
            if ( m_Dato>=node->m_Dato){
                return ;
            }
            node->m_Dato = m_Dato;
            padre = node->padre;
            if (padre!=nullptr && node->m_Dato < padre->m_Dato){
                cut(node, padre);
                cascadingCut(padre);
            }
            if (node->m_Dato < min->m_Dato)
                min = node;
        }

        void increase(Nodo<T> *node, T m_Dato)
        {
            Nodo<T> *m_Son, *padre, *right;
            if (min==nullptr ||node==nullptr)
                return ;
            if (m_Dato <= node->m_Dato){
                return ;
            }
            while (node->m_Son != nullptr){
                m_Son = node->m_Son;
                removeNode(m_Son);
                if (m_Son->right == m_Son)
                    node->m_Son = nullptr;
                else
                    node->m_Son = m_Son->right;
                addNode(m_Son, min);
                m_Son->padre = nullptr;
            }
            node->grado = 0;
            node->m_Dato = m_Dato;
            padre = node->padre;
            if(padre != nullptr){
                cut(node, padre);
                cascadingCut(padre);
            }
            else if(min == node){
                right = node->right;
                while(right != node){
                    if(node->m_Dato > right->m_Dato)
                        min = right;
                    right = right->right;
                }
            }
        }

        void update(Nodo<T> *node, T m_Dato)
        {
            if(m_Dato < node->m_Dato)
                decrease(node, m_Dato);
            else if(m_Dato > node->m_Dato)
                increase(node, m_Dato);
            else
                cout << "No necesita actualizarse" << endl;
        }

        Nodo<T>* search(Nodo<T> *root, T m_Dato)
        {
            Nodo<T> *t = root;
            Nodo<T> *p = nullptr;
            if (root==nullptr)
                return root;
            do{
                if (t->m_Dato == m_Dato){
                    p = t;
                    break;
                }
                else{
                    if ((p = search(t->m_Son, m_Dato)) != nullptr)
                        break;
                }
                t = t->right;
            } while (t != root);
            return p;
        }

        Nodo<T>* search(T m_Dato)
        {
            if (min==nullptr)
                return nullptr;
            return search(min, m_Dato);
        }

        void remove(Nodo<T> *node)
        {
            T m = min->m_Dato-1;
            decrease(node, m-1);
            removeMin();
        }

        void destroyNode(Nodo<T> *node)
        {
            Nodo<T> *start = node;
            if(node == nullptr)
                return;
            do{
                destroyNode(node->m_Son);
                node = node->right;
                delete node->left;
            } while(node != start);
        }

        void print(Nodo<T> *node, Nodo<T> *prev, int direction, ostream& out)
        {
            Nodo<T> *start=node;

            if (node==nullptr)
                return ;
            do{
                if (direction == 1)
                    out << prev->m_Dato << "->" << node->m_Dato << ";" << endl;
                else
                    out << prev->padre->m_Dato << "->" << node->m_Dato << ";" << endl;
                if (node->m_Son != nullptr)
                    print(node->m_Son, node, 1, out);
                prev = node;
                node = node->right;
                direction = 2;
            } while(node != start);
        }

};

int main()
{
    FibonacciHeap<int>* hb=new FibonacciHeap<int>();
    for(int i=1; i<=10000; i++){
        hb->insert(i);
    }
    cout << endl;
    hb->removeMin();
    hb->print(cout);

    ofstream file("FibonacciHeaps.dot");
    hb->print(file);
    return 0;
}