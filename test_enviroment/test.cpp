# include <iostream>
# include <queue>
using namespace std ;
class LegoBrick {
private :
double width , height ;
string color ;
public :
LegoBrick ( double width_in , double height_in )
: width ( width_in ) , height ( height_in ) {}
void printLegoBrick () const {
cout << "(" << width << " ," << height << " ) " ;
}
double getArea () {
return width * height ;
}
};
struct LegoBrickCompare {
bool operator ()( LegoBrick & a , LegoBrick & b ) const {
return a.getArea () > b.getArea ();
}
};


void DFS(){

}

void BFS(){
    
}




int main(){






}