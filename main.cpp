#include <fstream>
#include <list>
#include <unordered_map>

using namespace std;

ofstream g("heapuri.out");

struct nod  /// structura unui nod
{
    bool sters;
    int val, grad;
    nod *tata, *fiu_stanga, *frate_dreapta;
};
unordered_map<int, nod*> noduri;    /// lista cu valoarea nodului si pointerul catre valoarea respectiva

list<nod*> merge_heaps(list<nod*> &H1, list<nod*> &H2)
{
    list<nod*> aux;
    list<nod*>::iterator it1, it2, it3;

    it1 = H1.begin();
    it2 = H2.begin();
    while(it1 != H1.end() && it2 != H2.end())
        if((*it1)->grad <= (*it2)->grad)
        {
            aux.push_back(*it1);
            it1++;
        }
        else
        {
            aux.push_back(*it2);
            it2++;
        }
    while(it1 != H1.end())
    {
        aux.push_back(*it1);
        it1++;
    }
    while(it2 != H2.end())
    {
        aux.push_back(*it2);
        it2++;
    }   /// aux = lista cu arborii ordonati crescator dupa grad, care compun cele 2 heap-uri
    it1 = it2 = it3 = aux.begin();
    it2++;
    if(it2 != aux.end())
    {
        it3 = it2;
        it3++;
        while(it2 != aux.end())
            if((*it1)->grad < (*it2)->grad)
            {
                it1++;
                it2++;
                if(it3 != aux.end())
                    it3++;
            }
            else
                if(it3 != aux.end() && (*it1)->grad == (*it2)->grad && (*it2)->grad == (*it3)->grad)
                {   /// daca avem 3 arbori de acelasi grad, ii combinam pe al doilea si al treilea
                    it1++;
                    it2++;
                    it3++;
                }
                else
                    if((*it1)->grad == (*it2)->grad)
                    {   /// combinam 2 arbori de acelasi grad si refacem legaturile
                        if((*it1)->val > (*it2)->val)
                            swap(it1, it2);
                        (*it1)->frate_dreapta = (*it2)->frate_dreapta;
                        (*it2)->frate_dreapta = (*it1)->fiu_stanga;
                        (*it2)->tata = *it1;
                        (*it1)->fiu_stanga = *it2;
                        (*it1)->grad++;
                        aux.erase(it2);
                        it2 = it1;
                        it2++;
                        if(it3 != aux.end())
                            it3++;
                    }
    }
    return aux;
}

void insert(list<nod*> &H, const int &x)
{
    list<nod*>::iterator it1, it2;

    nod *nd = new nod;
    nd->sters = false;
    nd->val = x;
    nd->grad = 0;
    nd->tata = nd->fiu_stanga = nd->frate_dreapta = nullptr;    /// am creat un nod nou, cu valoarea x
    noduri[x] = nd;
    H.push_front(nd);   /// heap-ul va avea cel putin un element
    it1 = it2 = H.begin();
    it2++;
    if(it2 != H.end())  /// daca heap-ul are cel putin 2 elemente
        while(it2 != H.end())
        {
            if((*it1)->grad < (*it2)->grad)
            {
                it1++;
                it2++;
            }
            else
                if((*it1)->grad == (*it2)->grad)    /// atunci cand doi arbori au grade egale, ii unim intr-un nou arbore
                {                                   /// si refacem legaturile
                    if ((*it1)->val > (*it2)->val)
                        swap(*it1, *it2);
                    (*it1)->frate_dreapta = (*it2)->frate_dreapta;
                    (*it2)->frate_dreapta = (*it1)->fiu_stanga;
                    (*it2)->tata = *it1;
                    (*it1)->fiu_stanga = *it2;
                    (*it1)->grad++;
                    H.erase(it2);
                    it2 = it1;
                    it2++;
                }
        }
}

void build_heap(list<nod*> &H, const int v[1005], const int &n)
{
    int i;
    for(i = 0; i < n; i++)  /// construim un heap pe baza unui vector
        insert(H, v[i]);
}

void find_min(list<nod*> &H)
{
    int min1;
    list<nod*>::iterator it;

    min1 = 1 << 30;
    for(it = H.begin(); it != H.end(); it++)
        if(!(*it)->sters)
            min1 = min(min1, (*it)->val);   /// cautam minimul in lista de radacini
    g << "minimul este " << min1 << '\n';
}

void delete_nod(list<nod*> &H, const int &x)
{
    list<nod*> aux;
    list<nod*>::iterator it1;
    nod *nd, *auxnod;

    if(noduri.find(x) != noduri.end())
    {
        noduri[x]->sters = true;
        if(noduri[x]->tata == nullptr)   /// daca nodul cu valoarea x este radacina unui arbore, il stergem
        {
            for(it1 = H.begin(); it1 != H.end(); it1++)
                if((*it1)->val == x)
                    break;
            nd = (*it1)->fiu_stanga;
            H.erase(it1);
            while(nd != nullptr)
            {
                aux.push_front(nd);
                nd->tata = nullptr;
                auxnod = nd;
                nd = nd->frate_dreapta;
                auxnod->frate_dreapta = nullptr;    /// modificam legaturile fiilor nodului sters
            }
            noduri.erase(x);    /// elimin nodul din lista cu nodurile heap-ului
            H = merge_heaps(aux, H);    /// refac heap-ul
        }
    }
    for(it1 = H.begin(); it1 != H.end(); it1++)
        if((*it1)->sters)   /// daca au ajuns in lista de radacini noduri care trebuie sterse, le sterg pe rand
        {
            delete_nod(H, (*it1)->val);
            break;
        }
}

void delete_min(list<nod*> &H)
{
    int min1;
    list<nod*>::iterator it, it_min;

    min1 = 1 << 30;
    for(it = H.begin(); it != H.end(); it++)
        if(!(*it)->sters)   /// caut minimul si apelez delete_nod cu valoarea minimului
            min1 = min(min1, (*it)->val);
    delete_nod(H, min1);
}

void afisare_arbore(nod *nd)
{
    while(nd != nullptr)
    {
        if(!nd->sters)
            g << nd->val << ' ';
        afisare_arbore(nd->fiu_stanga);
        nd = nd->frate_dreapta;
    }
}

void afisare_heap(list<nod*> &H)
{
    list<nod*>::iterator it;

    for(it = H.begin(); it != H.end(); it++)
        afisare_arbore(*it);    /// afisez arborii care compun heap-ul
    if(H.begin() == H.end())
        g << "heap-ul este gol\n";
    g << "\n\n";
}

int main()
{
    ifstream f("heapuri.in");

    int n, i, nrop, op, x, v[1005];
    list<nod*> heap, heap1, heap2;

    f >> n;
    for(i = 0; i < n; i++)  /// citim n elemente pentru a forma heap-ul 1 cu operatia build
        f >> v[i];
    build_heap(heap1, v, n);    /// construiesc heap1

    f >> n;
    for(i = 0; i < n; i++)  /// asemanator formam heap-ul 2, asupra caruia aplicam diferite operatii
        f >> v[i];
    build_heap(heap2, v, n);    /// construiesc heap2
    f >> nrop;
    while(nrop)
    {
        f >> op;
        if(op <= 2)
            f >> x;

        if(op == 1)
            insert(heap2, x);
        else
            if(op == 2)
                delete_nod(heap2, x);
            else
                if(op == 3)
                    find_min(heap2);
                else
                    delete_min(heap2);

        nrop--;
    }

    g << "\nheap1: ";
    afisare_heap(heap1);
    g << "heap2: ";
    afisare_heap(heap2);
    heap = merge_heaps(heap1, heap2);   /// combinam heap1 si heap2, dupa care afisez rezultatul
    g << "heap-uri interclasate: ";
    afisare_heap(heap);

    f.close();
    g.close();

    return 0;
}
