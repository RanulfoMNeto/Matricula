#include <iostream>
#include <vector>
#include <exception>
#include <fstream>
#include <sstream>

using namespace std;

typedef pair<int, int> ii;
typedef vector<int> vi;
typedef vector<ii> vii;
typedef vector<vi> vvi;

class NodeNotFound : public exception {
    public:
        virtual const char* what() const throw() { 
            return "Node not found"; 
        } 
};

class ClassNotFound : public exception {
    public:
        virtual const char* what() const throw() { 
            return "Class not found"; 
        } 
};

class Codigo {
	public:
		string disciplina;
		string turma;
		Codigo() {};
		Codigo(string disciplina, string turma) {
			this->disciplina = disciplina;
			this->turma = turma;
		}
		bool operator==(Codigo codigo) {  
			return (disciplina == codigo.disciplina) and (turma == codigo.turma);
		}
};

class Node {
	public:
		Codigo codigo;
		string nome;
		string dia;
		int inicio;
		int fim;
		Node() {};

		Node(Codigo codigo, string nome, string dia, int inicio, int fim) {
			this->codigo = codigo;
			this->nome = nome;
			this->dia = dia;
			this->inicio = inicio;
			this->fim = fim;
		}

		bool operator==(Node node) {  
			return codigo == node.codigo;
		}
};

class Edge {
    public:
        Node *origem;
		Node *destino;
        Edge(Node &origem, Node &destino) {
            this->origem = &origem;
            this->destino = &destino;
        }
};

class Class {
	public:
		Codigo codigo;
		vector<Node*> T;
		Class (Codigo codigo) {
			this->codigo = codigo;
		}
		void add(Node &node) {
			T.push_back(&node);
		}
};

class Graph {
	public:
		vector<Node> V; // Conjunto de vértices (Coleta U Entrega U Depósito)
		vector<Class> C;
		vector<Edge> E;
		vi AL;

		Node& searchNodeByCode(Codigo codigo) {
			for (int i = 0; i < V.size(); i++) {
				if (V[i].codigo == codigo)
					return V[i];
			}
			throw NodeNotFound();
		}

		Class& searchClassByCode(Codigo codigo) {
			for (int i = 0; i < C.size(); i++) {
				if (C[i].codigo == codigo)
					return C[i];
			}
			throw ClassNotFound();
		}

		void addNode(string disciplina, string turma, string nome, string dia, int inicio, int fim) {
			Codigo codigo(disciplina, turma);
            Node vertice(codigo, nome, dia, inicio, fim);
            V.push_back(vertice);
        }

		void addEdge(Node &node1, Node &node2) {
			E.push_back(Edge(node1, node2));
		}

		void groupClassByCode() {
			for(int i = 0; i < V.size(); i++) {
				try {
					searchClassByCode(V[i].codigo).add(V[i]);
				} catch (ClassNotFound cnf) {
					C.push_back(V[i].codigo);
					searchClassByCode(V[i].codigo).add(V[i]);
				}
			}
		}

		void verifyConflicts() {
			groupClassByCode();

			// Turmas que possuem a mesma disciplina (externo)
			for (int i = 0; i < C.size(); i++) {
				for (int j = 0; (j < C.size()) and (j != i); j++) {
					if (C[i].codigo.disciplina == C[j].codigo.disciplina) {
						for (int m = 0; m < C[i].T.size(); m++) {
							for (int n = 0; n < C[j].T.size(); n++) {
								addEdge(*C[i].T[m], *C[j].T[n]);
								//addEdge(*C[j].T[m], *C[i].T[n]);
							}
						}
					}
				}
			}

			// Turmas que possuem o mesmo código(disciplina e turma) (interno)
			for (int i = 0; i < C.size(); i++) {
				for (int j = 0; j < C[i].T.size(); j++) {
					for (int m = 0; (m < C[i].T.size()) and (m != j); m++) {
						addEdge(*C[i].T[j], *C[i].T[m]);
						//addEdge(*C[i].T[m], *C[i].T[j]);
					}
				}
			}

			// Turmas que possuem conflito de horários
			for (int i = 0; i < C.size(); i++) {
				for (int j = 0; (j < C.size()) and (j != i); j++) {
					bool conflict = false;
					for (int m = 0; m < C[i].T.size(); m++) {
						for (int n = 0; n < C[j].T.size(); n++) {
							if (C[i].T[m]->dia == C[j].T[n]->dia) {
								if ((C[i].T[m]->inicio < C[j].T[n]->inicio) and (C[i].T[m]->fim > C[j].T[n]->inicio)) {
									conflict = true;
								} else if ((C[i].T[m]->inicio < C[j].T[n]->fim) and (C[i].T[m]->fim > C[j].T[n]->fim)) {
									conflict = true;
								} else if ((C[i].T[m]->inicio > C[j].T[n]->inicio) and (C[i].T[m]->fim < C[j].T[n]->fim)) {
									conflict = true;
								}
							}
						}
					}
					if (conflict) {
						for (int m = 0; m < C[i].T.size(); m++) {
							for (int n = 0; n < C[j].T.size(); n++) {
								addEdge(*C[i].T[m], *C[j].T[n]);
								//addEdge(*C[j].T[m], *C[i].T[n]);
							}
						}
					}
				}
			}
		}

		void printConflicts() {
			for (int i = 0; i < E.size(); i++) {
				cout << "(" << E[i].origem->codigo.disciplina << "," << E[i].origem->codigo.turma << ")" << E[i].origem->inicio << " | ";
				cout << "(" << E[i].destino->codigo.disciplina << "," << E[i].destino->codigo.turma << ")" << E[i].destino->inicio << endl;
			}
		}

};

void lerArquivo(string nomeArquivo, Graph &graph) {
    ifstream arquivo(nomeArquivo+".txt");
    string linha;

    getline(arquivo, linha); // header
    while(getline(arquivo, linha)) {
        stringstream sst(linha);
        string info;
		int i = 0;

		string disciplina;
		string turma;
		string nome;
		string dia;
		int inicio;
		int fim;

        while(getline(sst, info, ',')) {
            if (i == 0) {
				disciplina = info;
			} else if (i == 1) {
				turma = info;
			} else if (i == 2) {
				nome = info;
			} else if (i == 3) {
				dia = info;
			} else if (i == 4) {
				inicio = stoi(info);
			} else if (i == 5) {
				fim = stoi(info);
			}
			i++;
        }
        graph.addNode(disciplina, turma, nome, dia, inicio, fim);
    }
}

int main() {

	string nomeArquivo = "disciplinas";
	//cout << "Nome do arquivo: ";
	//cin >> nomeArquivo;

	Graph graph;
	lerArquivo(nomeArquivo, graph);

	graph.verifyConflicts();
	graph.printConflicts();

	return 0;
}
