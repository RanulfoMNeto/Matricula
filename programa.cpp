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

class EdgeNotFound : public exception {
    public:
        virtual const char* what() const throw() { 
            return "Edge not found"; 
        } 
};

class ClassNotFound : public exception {
    public:
        virtual const char* what() const throw() { 
            return "Class not found"; 
        } 
};

class CodeNotFound : public exception {
    public:
        virtual const char* what() const throw() { 
            return "Code not found"; 
        } 
};

class Code {
	public:
		string disciplina;
		string turma;
		Code() {};
		Code(string disciplina, string turma) {
			this->disciplina = disciplina;
			this->turma = turma;
		}
		bool operator==(Code code) {  
			return (disciplina == code.disciplina) and (turma == code.turma);
		}
};

class Node {
	public:
		Code code;
		string nome;
		string dia;
		int inicio;
		int fim;
		Node() {};

		Node(Code code, string nome, string dia, int inicio, int fim) {
			this->code = code;
			this->nome = nome;
			this->dia = dia;
			this->inicio = inicio;
			this->fim = fim;
		}

		bool operator==(Node node) {  
			return code == node.code;
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
		Code code;
		vector<Node*> T;
		Class (Code code) {
			this->code = code;
		}
		void add(Node &node) {
			T.push_back(&node);
		}
};

typedef vector<Code> vc;
typedef pair<Code,vc> cvc;
typedef vector<cvc> vcvc;

bool degreePriority(ii p1, ii p2) {
	return p1.second > p2.second;
}

class Graph {
	public:
		vector<Node> V; // Conjunto de vértices (Coleta U Entrega U Depósito)
		vector<Class> C;
		vector<Edge> E;
		vcvc AL;
		vvi ALi;

		Class& searchClassByIndex(int index) {
			return C[index];
			throw ClassNotFound();
		}

		Node& searchNodeByCode(Code code) {
			for (int i = 0; i < V.size(); i++) {
				if (V[i].code == code)
					return V[i];
			}
			throw NodeNotFound();
		}

		Edge& searchEdgeByCode(Code code1, Code code2) {
			for (int i = 0; i < E.size(); i++) {
				if ((E[i].origem->code == code1) and (E[i].destino->code == code2))
					return E[i];
			}
			throw EdgeNotFound();
		}

		Class& searchClassByCode(Code code) {
			for (int i = 0; i < C.size(); i++) {
				if (C[i].code == code)
					return C[i];
			}
			throw ClassNotFound();
		}

		void addNode(string disciplina, string turma, string nome, string dia, int inicio, int fim) {
			Code code(disciplina, turma);
            Node vertice(code, nome, dia, inicio, fim);
            V.push_back(vertice);
        }

		void addEdge(Node &node1, Node &node2) {
			E.push_back(Edge(node1, node2));
		}

		void groupClassByCode() {
			for(int i = 0; i < V.size(); i++) {
				try {
					searchClassByCode(V[i].code).add(V[i]);
				} catch (ClassNotFound cnf) {
					C.push_back(V[i].code);
					searchClassByCode(V[i].code).add(V[i]);
				}
			}
		}

		void findConflicts() {
			groupClassByCode();

			/*
			// Conflito de disciplinas (disciplina com turmas iguais)
			for (int i = 0; i < C.size(); i++) {
				for (int m = 0; m < C[i].T.size(); m++) {
					for (int n = 0; (n < C[i].T.size()) and (m != n); n++) {
						try {
							searchEdgeByCode(C[i].T[m]->code, C[i].T[n]->code);
						} catch (EdgeNotFound enf) {
							try {
								searchEdgeByCode(C[i].T[n]->code, C[i].T[m]->code);
							} catch (EdgeNotFound enf) {
								addEdge(*C[i].T[m], *C[i].T[n]);
								addEdge(*C[i].T[n], *C[i].T[m]);
							}
						}
					}
				}
			}
			*/

			// Turmas que possuem conflito de horários
			for (int i = 0; i < C.size(); i++) {
				for (int j = 0; (j < C.size()) and (i != j); j++) {
					bool conflict = false;
					if (C[i].code.disciplina == C[j].code.disciplina) {
						conflict = true;
					} else {
						for (int m = 0; m < C[i].T.size(); m++) {
							for (int n = 0; n < C[j].T.size(); n++) {
								if (C[i].T[m]->dia == C[j].T[n]->dia) {
									if ((C[i].T[m]->inicio <= C[j].T[n]->inicio) and (C[i].T[m]->fim >= C[j].T[n]->inicio)) {
										conflict = true;
									} else if ((C[i].T[m]->inicio <= C[j].T[n]->fim) and (C[i].T[m]->fim >= C[j].T[n]->fim)) {
										conflict = true;
									} else if ((C[i].T[m]->inicio >= C[j].T[n]->inicio) and (C[i].T[m]->fim <= C[j].T[n]->fim)) {
										conflict = true;
									}
								}
							}
						}
					}
					if (conflict) {
						for (int m = 0; m < C[i].T.size(); m++) {
							for (int n = 0; n < C[j].T.size(); n++) {
								try {
									searchEdgeByCode(C[i].code, C[j].code);
									searchEdgeByCode(C[j].code, C[i].code);
								} catch (EdgeNotFound enf) {
									
										addEdge(*C[i].T[m], *C[j].T[n]);
										addEdge(*C[j].T[n], *C[i].T[m]);
								}
							}
						}
					}
				}
			}

		}

		void printConflicts() {
			for (int i = 0; i < E.size(); i++) {
				cout << "(" << E[i].origem->code.disciplina << "," << E[i].origem->code.turma << ")" << E[i].origem->inicio << " | ";
				cout << "(" << E[i].destino->code.disciplina << "," << E[i].destino->code.turma << ")" << E[i].destino->inicio << endl;
			}
		}

		int searchALIndexByCode(Code code) {
			for (int i = 0; i < AL.size(); i++) {
				if (AL[i].first == code)
					return i;
			}
			throw CodeNotFound();
		}

		cvc& searchALByCode(Code code) {
			for (int i = 0; i < AL.size(); i++) {
				if (AL[i].first == code)
					return AL[i];
			}
			throw CodeNotFound();
		}

		void adjacencyListGenerator() {
			for (int i = 0; i < C.size(); i++) {
				AL.push_back(cvc(C[i].code, vc()));
			}

			for (int i = 0; i < E.size(); i++) {
				searchALByCode(E[i].origem->code).second.push_back(E[i].destino->code);
			}
		}

		void printAL() {
			for (int i = 0; i < AL.size(); i++) {
				cout << endl << "[" << AL[i].first.disciplina << "-" << AL[i].first.turma << "]" << endl;
				for (int j = 0; j < AL[i].second.size(); j++) {
					cout << "    " << AL[i].second[j].disciplina << "-" << AL[i].second[j].turma << ";" << endl;
				}
			}
		}

		void convertAdjacencyList() {
			ALi = vvi(AL.size());
			for (int i = 0; i < AL.size(); i++) {
				for (int j = 0; j < AL[i].second.size(); j++) {
					ALi[i].push_back(searchALIndexByCode(AL[i].second[j]));
				}
			}
		}

		int degree(int u) {
			return ALi[u].size();
		}

		vi sortNodesByDregree() {
			vii aux;
			for (int i = 0; i < ALi.size(); i++) {
				aux.push_back(ii(i, degree(i))); // <id, degree>
			}
			sort(aux.begin(), aux.end(), degreePriority);
			vi L;
			for (int i = 0; i < aux.size(); i++) {
				int v = aux[i].first;
				L.push_back(v);
			}
			return L;
		}

		vi coloringHeuristic() {
			adjacencyListGenerator();
			convertAdjacencyList();
			// Seja L uma lista de vértices ordenada de forma descendente pelo grau;
			vi L = sortNodesByDregree();
			
			// c ← 1;
			int c = 1;
			vi color(ALi.size(), -1);
			
			// while L ≠ ∅ do
			while(!L.empty()) {
				// for all v ∈ L tal que v não é colorido do
				for (int i = 0; i < L.size(); i++) {
					bool colored = false;
					int v = L[i];
					for (int j = 0; j < ALi[v].size(); j++) {
						int u = ALi[v][j];
						colored = (color[u] == c)?true:false;
					}
					// if nenhum vértice adjacente a v foi colorido com c then
					if (!colored) {
						// Colora v com a color c;
						color[v] = c;
					}
				}
				for (int i = 0; i < L.size(); i++) {
					int v = L[i];
					if (color[v] != -1) {
						L.erase(L.begin()+i);
						i--;
					}
				}
				// c ← c + 1;
				c++;
			}
			return color;
		}

		void printClass() {
			for (int i = 0; i < C.size(); i++) {
				cout << endl << C[i].code.disciplina << "-" << C[i].code.turma << endl << endl;
				for (int j = 0; j < C[i].T.size(); j++) {
					cout << C[i].T[j]->code.disciplina << endl;
				}
			}
		}

};

void lerArquivo(string fileName, Graph &graph) {
    ifstream arquivo(fileName+".txt");
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
	string fileName = "disciplinas";

	Graph graph;
	lerArquivo(fileName, graph);

	graph.findConflicts();
	graph.printClass();
	graph.printConflicts();

	vi color = graph.coloringHeuristic();

	for (int i = 0; i < color.size(); i++) {
		cout << graph.searchClassByIndex(i).code.disciplina << "-" << graph.searchClassByIndex(i).code.turma << ":" << color[i] << endl;
	}

	graph.printAL();
	
	return 0;
}
