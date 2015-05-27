#include <string>
#include <vector>
#include "matrix.hpp"
using namespace std;

/**
 * Class for reading a .tp file
 */
class TpReader
{
public:
  
  /**
   * Initialize
   * @param filename File to read
   */
  TpReader(string filename)
  {
    setFilename(filename);
    m_matrices = vector<Matrix4>();
  }

  /**
   * Set the filename to read
   * @param filename File to read
   */
  inline void setFilename(string filename)
  {
    m_filename = filename;
  }
  
  /**
   * Get the filename to read
   * @return filename to read
   */
  inline string getFilename()
  {
    return m_filename;
  }
  /**
   * Read the file
   */
  void read();

  /**
   * Return the contents of the .tp file as a vector of Eigen 4x4 matrices
   * @return the matrices
   */
  inline vector<Matrix4> getMatrices()
  {
    return m_matrices;
  }
  
private:
  string m_filename;
  vector<Matrix4> m_matrices;

};
  
