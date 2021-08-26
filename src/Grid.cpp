#pragma once
#include <cstring>

// Generic grid class. An image is, for example, a grid of pixels, while an
// energy map is a grid of energies (duh).
template <class T>
class Grid {
  protected:
    int h, w;
    T  *cells;

  public:
    // empty Grid constructor.
    Grid(int _h, int _w) : h(_h), w(_w), cells(new T[h * w]) { }


    // copy constructor (performs a deep copy).
    Grid(Grid &other) : h(other.h), w(other.w), cells(new T[h * w]) {
      std::memcpy(cells, other.cells, h * w * sizeof(T));
    }


    // construct Grid from given cells.
    Grid(int _h, int _w, T *_cells) : h(_h), w(_w), cells(new T[h * w]) {
      std::memcpy(cells, _cells, h * w * sizeof(T));
    }


    ~Grid() {
      delete[] cells;
    }

    // move (assignment) operator.
    Grid& operator=(Grid &&other) {
      if (this != &other) {
        delete[] cells;

        cells = other.cells;
        h     = other.h;
        w     = other.w;

        // TODO: should other.h and other.w also be cleared? probably not.
        other.cells = nullptr;
      }
      return *this;
    }


    T  *getCells()  { return cells; }
    int getHeight() { return h;     }
    int getWidth()  { return w;     }


    bool operator ==(Grid &other);
    bool operator !=(Grid &other);

    std::ostream& operator <<(std::ostream &os);
    void printGrid();
};

template <class T>
bool Grid<T>::operator ==(Grid<T> &other) {
  int h = getHeight();
  int w = getWidth();

  if (h != other.getHeight() || w != other.getWidth())
    return false;

  T *cells1 = getCells();
  T *cells2 = other.getCells();

  for (int i = 0; i < h * w; i++)
    if (cells1[i] != cells2[i])
      return false;

  return true;
}



template <class T>
std::ostream& operator <<(std::ostream &os, Grid<T> &grid) {

  T *cells = grid.getCells();
  int h = grid.getHeight();
  int w = grid.getWidth();

  os << "[\n";
  for (int i = 0; i <  h - 1; i++) {
    os << " [";
    for (int j = 0; j <  w - 1; j++)
      os << cells[i * w + j] << ", ";
    if (w > 0) os << cells[(i+1) * w - 1] << "], \n";
    else       os << "], \n";
  }
  if (h > 0) {
    os << " [";
    for (int j = 0; j < w - 1; j++)
      os << cells[(h - 1) * w + j] << ", ";
    if (w > 0) os << cells[h * w - 1] << "]\n";
    else       os << "]\n";
  }
  os << "]\n";
  return os;
}
