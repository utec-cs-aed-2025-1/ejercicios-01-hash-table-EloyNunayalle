#include <vector>

using namespace std;

const int maxColision = 3;
const float maxFillFactor = 0.8;

template<typename TK, typename TV>
struct ChainHashNode {
   TK key;
   TV value;
   ChainHashNode* next;

   ChainHashNode() : key(TK()), value(TV()), next(nullptr) {}
   ChainHashNode(TK key, TV value) {
    this->key = key;
    this->value = value;
    next = nullptr;
   }
};

template<typename TK, typename TV>
class ChainHashListIterator {
    private:
        ChainHashNode<TK,TV>* current;
    public:
        ChainHashListIterator(ChainHashNode<TK,TV>* current) {
            this->current = current;
        }

        ChainHashListIterator& operator++() {
            this->current = current->next;
            return *this;
        }

        ChainHashListIterator operator++(int) {
            ChainHashListIterator copia = *this;
            this->current = current->next;
            return copia;
        }

        bool operator!=(ChainHashListIterator otro) {
            return this->current != otro.current;
        }

       bool operator==(ChainHashListIterator otro) {
           return this->current == otro.current;
       }

       ChainHashNode<TK,TV>& operator*() {
           return *this->current;
       }

};

template<typename TK, typename TV>
class ChainHash
{
private:    
    typedef ChainHashNode<TK, TV> Node;
    typedef ChainHashListIterator<TK, TV> Iterator;

	Node** array;  // array de punteros a Node
    int nsize; // total de elementos <key:value> insertados
	int capacity; // tamanio del array
	int *bucket_sizes; // guarda la cantidad de elementos en cada bucket
	int usedBuckets; // cantidad de buckets ocupados (con al menos un elemento)

public:
    ChainHash(int initialCapacity = 10){
		this->capacity = initialCapacity; 
		this->array = new Node*[capacity]();  
		this->bucket_sizes = new int[capacity]();
		this->nsize = 0;
		this->usedBuckets = 0;
	}

	TV get(TK key){
		size_t hashcode = getHashCode(key);
		size_t index = hashcode % capacity;
		
		Node* current = this->array[index];
		while(current != nullptr){
			if(current->key == key) return current->value;
			current = current->next;
		}
		throw std::out_of_range("Key no encontrado");
	}
	
	int size(){ return this->nsize; }	

	int bucket_count(){ return this->capacity; }

	int bucket_size(int index) { 
		if(index < 0 || index >= this->capacity) throw std::out_of_range("Indice de bucket invalido");
		return this->bucket_sizes[index]; 
	}	
	
	// TODO: implementar los siguientes métodos
	void set(TK key, TV value) {
        size_t hashcode = getHashCode(key);
        int index = hashcode % capacity;


        Node* existe = find(array[index], key);
        if (existe != nullptr) {
            existe->value = value;
        } else {
            if (array[index] == nullptr) // aumentamos la cantidad de buckets ocupados si esta vacio el bucket
                usedBuckets += 1;

            Node* newNode = new Node(key, value);
            newNode->next = array[index];
            array[index] = newNode;
            
            nsize += 1;
            bucket_sizes[index] += 1;
        }



        if (fillFactor() > maxFillFactor || bucket_sizes[index] > maxColision) {
            rehashing();
        }
    };

	bool remove(TK key) {
        size_t hashcode = getHashCode(key);
        int index = hashcode % capacity;

        if (array[index] == nullptr)
            return false; // throw std::runtime_error("No existe");
        else if (bucket_sizes[index] == 1) {
            if (array[index]->key != key)
                return false; // throw std::runtime_error("No existe");
            delete array[index];
            array[index] = nullptr;
            --usedBuckets;
        } else {
            Node* current = array[index];
            while (current->next != nullptr && current->next->key != key)
                current =  current->next;
            if (current->next == nullptr)
                return false; // throw std::runtime_error("No existe");
            
            Node* temp = current->next;
            current->next = current->next->next;
            delete temp;
        }

        --bucket_sizes[index];
        --nsize;
        return true;
    }

	bool contains(TK key) {
        size_t hashcode = getHashCode(key);
        int index = hashcode % capacity;

        return exists(array[index], key);
    }	

	Iterator begin(int index) {
        return Iterator(this->array[index]);
    }
    
	Iterator end(int index) {
        return Iterator(nullptr);
    }

private:
	double fillFactor(){
		return (double)this->usedBuckets / (double)this->capacity;
	}


	size_t getHashCode(TK key) {
		std::hash<TK> ptr_hash;
		return ptr_hash(key);
	}

	//TODO: implementar rehashing
	void rehashing() {
        int newCapacity = capacity * 2; // nueva capacidad

        Node** newArray = new Node*[newCapacity](); // nuevo array 

        // reiniciando bucket sizes;
        delete[] bucket_sizes;
        bucket_sizes = new int[newCapacity]();

        usedBuckets = 0; // reiniciando cantidad de buckets usados

        // reubicando cada ChainHashNode en el nuevo array;
        for (int i = 0; i< capacity; ++i) {

            Node*& current = array[i];

            while (current != nullptr) {
                size_t hashcode = getHashCode(current->key);
                int index = hashcode % newCapacity;

                Node* temp = current;

                // pop_front en bucket de índice i de array
                current = current->next;

                // push_front en bucket de índice j de newArray
                temp->next = newArray[index];
                newArray[index] = temp;

                if (bucket_sizes[index] == 0)
                   ++usedBuckets;

                bucket_sizes[index] += 1; // aumento cantidad de elementos en el bucket index
            }
        }

        delete[] array; // elimino array antiguo
        array = newArray; // asigno el nuevo array
        capacity = newCapacity; // actualizo capacidad

    }

    bool exists(Node* node, TK key) {
        Node* current = node;
        while (current != nullptr) {
            if (current->key == key)
                return true;
            current =  current->next;
        }
        return false;
    }

    Node* find(Node* node, TK key) {
        Node* current = node;
        while (current != nullptr) {
            if (current->key == key)
                return current;
            current = current->next;
        }
        return nullptr;
    }



public:
	// TODO: implementar destructor
	~ChainHash() {
        for (int i = 0; i < capacity; ++i) {
            Node* current = array[i];
            while (current != nullptr) {
                Node* temp = current;
                current = current->next;
                delete  temp;
            }
        }

        delete[] array;
        delete[] bucket_sizes;
    };
};
