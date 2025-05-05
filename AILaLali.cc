#include "Player.hh"
#include <iostream>
#include <vector>
#include <string>
#include <set>
using namespace std;

/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME LaLali


struct PLAYER_NAME : public Player {

  /**
   * Factory: returns a new instance of this class.
   * Do not modify this function.
   */
  static Player* factory () {
    return new PLAYER_NAME;
  }

  /**
   * Types and attributes for your player can be defined here.
   */
    typedef vector<vector<char>> Tablero;
    Tablero subterraneo;//tablero que indica en que posiciones hay cuevas y en que posiciones hay ascesores y furyans y pioneros en cada ronda
    Tablero subterraneo_oficial;
    
    vector<int> pioneros_ext; //id pioneros en el exterior
    vector<int> pioneros_sub; //id pioneros en el subterraneo
    vector<int> furyans_ext; //id furyans exterior
    vector<int> furyans_sub; //id furyans subterraneo
    int fila_sol_d = -1;
    int fila_sol_i = -1; //extremos del sol(ambos incluidos);
    set<Pos> gemas; //posiciones donde se encuentran las gemas;
    int inf = 1000;
    
    vector<pair<int,int>> dirs = {{1,0},{1,1},{0,1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1}};
    
    bool pos_ok(Pos p, const vector<vector<int>>& dis){
    return p.i < 40 and p.j < 80 and p.i >= 0 and p.j >= 0 and dis[p.i][p.j]==inf and subterraneo[p.i][p.j] != 'R'; 
}
    
        
    //lee donde esta el sol, los ascensores, las cuevas y las gemas al iniciar el juego
    void leer_tablero(){
        for (int i = 0; i < 40; ++i) {
            for (int j = 0; j < 80; ++j) {
                if (i == 0 and j != 0){ //calcula extremos del sol
                    Pos p1(i,j-1,1);
                    Pos p(i,j,1);
                    if (!daylight(p1) and daylight(p)){
                        fila_sol_i = j;
                        fila_sol_d = (j+39)%80;
                    }
                    if (fila_sol_d == -1 and j == 79){
                        fila_sol_i = 0;
                        fila_sol_d = 39;
                    }
                }
                Cell c = cell(i,j,0);
                if (c.type == Rock){ //mira si es una roca
                    subterraneo[i][j] = 'R';
                }
                else if (c.type == Elevator){ //mira si es un ascensor
                    subterraneo[i][j] = 'E';
                }
                if (c.gem){ //mira si en la casilla exterior hay una gema
                    Pos p(i,j,1);
                    gemas.insert(p);
                }
            }
        }
    }
    
    //guarda las posiciones de los pioneros y los furyans de mi jugador al empezar la ronda
    void leer_unidades(){
        vector<int> P = pioneers(me());
        for (int id : P) {
            if (unit(id).pos.k == 0) pioneros_sub.push_back(id);
            else pioneros_ext.push_back(id);
        }
        
        vector<int> F = furyans(me());
        for (int id : F) {
            if (unit(id).pos.k == 0) furyans_sub.push_back(id);
            else furyans_ext.push_back(id);
        }
    }
    
    
Dir movimiento (Pos inicial, Pos fin){
    if (inicial.i < fin.i and inicial.j == fin.j and inicial.k == fin.k) return Bottom;
    else if (inicial.i < fin.i and inicial.j < fin.j and inicial.k == fin.k) return BR;
    else if (inicial.i == fin.i and inicial.j < fin.j and inicial.k == fin.k) return Right;
    else if (inicial.i > fin.i and inicial.j < fin.j and inicial.k == fin.k) return RT;
    else if (inicial.i > fin.i and inicial.j == fin.j and inicial.k == fin.k) return Top;
    else if (inicial.i > fin.i and inicial.j > fin.j and inicial.k == fin.k) return TL;
    else if (inicial.i == fin.i and inicial.j > fin.j and inicial.k == fin.k) return Left;
    else if (inicial.i < fin.i and inicial.j > fin.j and inicial.k == fin.k) return LB;
    else if (inicial.i == fin.i and inicial.j == fin.j and inicial.k <  fin.k) return Up;
    else if (inicial.i == fin.i and inicial.j == fin.j and inicial.k > fin.k) return Down;
    else return None;
}

    Pos huir(Pos actual, Dir mejor){ //posicion de la unidad y posicion de la celda a la que quiere huir
        int idx = mejor;
        pair<int, int> mov = dirs[idx];
        for (int i = 0; i < 5;++i){
            Pos p;
                p.i = actual.i+mov.first;
                p.j = actual.j+mov.second;
                p.k = actual.k;
            
            if (p.i < 40 and p.j < 80 and p.i >= 0 and p.j >= 0){
                Cell c = cell(p);
                if ((p.k == 0 and subterraneo[p.i][p.j] == '.' and c.id == -1) or p.k == 1) return p ;//huyes a una casilla si estas en el exterior o en el subterraneo y la casilla no esta ocupada ni es una roca
            } //calcula la siguiente mejor casilla a la que huir si a la anterior no puedes
            if (i == 0) mov = dirs[(idx+1)%8];
            else if (i == 1) mov = dirs[(idx-1)%8];
            else if (i == 2) mov = dirs[(idx+2)%8];
            else mov = dirs[(idx-2)%8];
        
        }
        return actual;
    }
    
    //calcula cual es el pionero exterior mas cercano a cada gema y lo envia a por ella
    void busca_gemas(){
        for (auto g : gemas){
            int dist_min; //distancia minima del pionero a la gema
            int id_min = -1; //id del pionero mas cercano
            for (int p = 0; p < 4; ++p){
                if (id_min == -1){ //si es el primero
                    dist_min = max(abs(g.i-unit(pioneros_ext[p]).pos.i), abs(g.j-unit(pioneros_ext[p]).pos.j));
                    id_min = pioneros_ext[p];
                }
                else if (dist_min > max(abs(g.i-unit(pioneros_ext[p]).pos.i), abs(g.j-unit(pioneros_ext[p]).pos.j))){
                    dist_min = max(abs(g.i-unit(pioneros_ext[p]).pos.i), abs(g.j-unit(pioneros_ext[p]).pos.j));
                    id_min = pioneros_ext[p];
                } 
            }
           
            command(id_min, movimiento(unit(id_min).pos, g));
        }
    }
    
    int camino_bfs(const Tablero& t, Pos inicial, vector<vector<Pos>>& prev, Pos& fin, char objetivo){
        if (t[inicial.i][inicial.j] == objetivo) return 0;
        vector<vector<int>> dis(40,vector<int>(80, inf));
        queue<Pos> Q;
        Q.push(inicial);
        dis[inicial.i][inicial.j] = 0;
        while (not Q.empty()){
            Pos act = Q.front();
            Q.pop();
            for(auto d : dirs){
                Pos aux;
                aux.k = act.k;
                aux.j = act.j + d.second;
                aux.i = act.i + d.first;
                if (pos_ok(aux, dis)){
                    Q.push(aux);
                    dis[aux.i][aux.j] = dis[act.i][act.j]+1;
                    prev[aux.i][aux.j] = act;
                    if (t[aux.i][aux.j] == objetivo) {
                        fin = aux;
                        return dis[aux.i][aux.j];
                    }
                }
            }
        }
        fin.i = -1;
        fin.j = -1;
        fin.k = -1;
        return -1;
    }
    
    Dir camino_bfs(const Tablero& t,Pos inicial, char objetivo){
        Unit u = unit(cell(inicial).id);
        vector<vector<Pos>> prev(40, vector<Pos>(80));
        Pos fin;
        int d = camino_bfs(t, inicial, prev, fin, objetivo);
        if (d == -1 or (objetivo == 'H' and d > 6))return None;
        else if (objetivo == 'E' and d == 0) return Up;
        else if (objetivo == 'F' and d < 3 and (u.type == Pioneer or (u.type == Furyan and u.health <= unit(cell(fin).id).health ))) return movimiento(inicial, huir(inicial,movimiento(fin,inicial)));
        else if (objetivo == 'F' and d < 3 and u.type == Furyan) return movimiento(inicial,fin);
        else if (objetivo == 'F' and (u.type == Pioneer or u.type == Furyan)) return None;
        else {
            Pos previo = prev[fin.i][fin.j];
            while (previo.i != inicial.i or previo.j != inicial.j){
                fin = previo;
                previo = prev[fin.i][fin.j];
            }
            if (objetivo == 'H') return movimiento(inicial, huir(inicial,movimiento(fin,inicial)));;
            return movimiento(inicial, fin);
        }
    }
    
                            
    void mover_pioneros(){
        for(uint i = 0; i < pioneros_sub.size(); ++i){
             Dir dir = camino_bfs(subterraneo, unit(pioneros_sub[i]).pos, 'H'); //si un hellhound esta a menos de 4 posiciones, huir
             if (dir != None) command(pioneros_sub[i], dir);
             else { //els que no estan cerca de un hellhound, huir de los furyans
                 dir = camino_bfs(subterraneo, unit(pioneros_sub[i]).pos, 'F');
                 if (dir != None) command(pioneros_sub[i], dir);
                 else { //si no los persigue nadie                    
                        
                        for(auto d : dirs){
                            Pos aux;
                            aux.k = unit(pioneros_sub[i]).pos.k;
                            aux.j = unit(pioneros_sub[i]).pos.j + d.second;
                            aux.i = unit(pioneros_sub[i]).pos.i + d.first;
                            if (aux.i < 40 and aux.j < 80 and aux.i >= 0 and aux.j >= 0 and subterraneo[aux.i][aux.j] != 'R' and subterraneo[aux.i][aux.j] != 'E'){ //si la posicion esta dentro del tablero y no es una roca ni ascensor
                                Cell c = cell(aux);
                                if (c.owner != me() and c.id == -1) command(pioneros_sub[i], movimiento(unit(pioneros_sub[i]).pos, aux)); //celda no ocupada de enemigo
                                else if (c.owner == -1 and c.id == -1) dir = movimiento(unit(pioneros_sub[i]).pos, aux); //celda de nadie no ocupada
                                
                            }
                        }
                        if (dir == None){
                            int idx = random(0,7);
                            Pos aux (unit(pioneros_sub[i]).pos.i+dirs[idx].first, unit(pioneros_sub[i]).pos.j+dirs[idx].second, unit(pioneros_sub[i]).pos.k);
                            dir = movimiento(unit(pioneros_sub[i]).pos, aux);
                        }
                        command(pioneros_sub[i], dir);
                    }
            }
        }
    }
    
    void mover_furyans(){
        for(uint i = 0; i < furyans_sub.size(); ++i){
             Dir dir = camino_bfs(subterraneo, unit(furyans_sub[i]).pos, 'H'); //si un hellhound esta a menos de 4 posiciones, huir
             if (dir != None) command(furyans_sub[i], dir);
             else { //els que no estan cerca de un hellhound, ir a por furyans enemigos
                 dir = camino_bfs(subterraneo, unit(furyans_sub[i]).pos, 'F');
                 if (dir != None) command(furyans_sub[i], dir);
                 dir = camino_bfs(subterraneo, unit(furyans_sub[i]).pos, 'P');
                 command(furyans_sub[i], dir);
             }
        }
    }
                 
  
  /**
   * Play method, invoked once per each round.
   */
  virtual void play () {
      if (round() == 0){
        subterraneo = Tablero (40, vector<char>(80, '.')); //matriz que guarda las posiciones donde hay rocas y ascesores. en cada ronda se actualizan la posicoines de los 3 hellhounds.
        leer_tablero();
        subterraneo_oficial = subterraneo;
      }
      else {
        fila_sol_d += 2;
        fila_sol_i += 2;
      }
      leer_unidades();
      busca_gemas();
      for (int h = 0; h < 3; ++h) subterraneo[unit(hellhounds()[h]).pos.i][unit(hellhounds()[h]).pos.j] = 'H'; //marcar donde estan los hellhounds en esta ronda
      
      for (int i = 0; i < 4; ++i){
          if (i != me()){
                vector<int> P = pioneers(i);
                for (int id : P) {
                    if (unit(id).pos.k == 0) subterraneo[unit(id).pos.i][unit(id).pos.j]= 'P';
                }
                
                vector<int> F = furyans(i);
                for (int id : F) {
                    if (unit(id).pos.k == 0) subterraneo[unit(id).pos.i][unit(id).pos.j] = 'F';
                    
                }
          }
        }
                    
      mover_pioneros();
      mover_furyans();
      
      furyans_ext.clear();
      furyans_sub.clear();
      pioneros_ext.clear();
      pioneros_sub.clear();
      subterraneo = subterraneo_oficial;
  }

};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
