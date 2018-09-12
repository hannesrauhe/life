#include <iostream>
#include <set>
#include <map>
#include <thread>
#include <chrono>
#include <cstring>

typedef unsigned int uint;
typedef std::map< uint, std::set<uint> > field_t;

class Game {
public:
  Game(std::istream& in);
  void printField() const;

  void setLive(uint r, uint c)
  {
    alive_[r].insert(c);
  }

  bool isAlive(uint r, uint c) const
  {
    auto it = alive_.find(r);
    if(it==alive_.end()) return false;
    return it->second.find(c)!=it->second.end();
  }

  bool isAlive() const
  {
    return !alive_.empty();
  }


  bool checkPotential(uint r, uint c)
  {
    if(isAlive(r,c)) return true;
    potential_newborn_[r].insert(c);
    return false;
  }

  uint countNeighbors(uint r, uint c)
  {
    auto n = 0;
    if(checkPotential(r-1,c-1)) n++;
    if(checkPotential(r-1,c)) n++;
    if(checkPotential(r-1,c+1)) n++;
    if(checkPotential(r,c-1)) n++;
    if(checkPotential(r,c+1)) n++;
    if(checkPotential(r+1,c-1)) n++;
    if(checkPotential(r+1,c)) n++;
    if(checkPotential(r+1,c+1)) n++;
    return n;
  }

  uint countNeighborsOfDead(uint r, uint c) const
  {
    auto n = 0;
    if(isAlive(r-1,c-1)) n++;
    if(isAlive(r-1,c)) n++;
    if(isAlive(r-1,c+1)) n++;
    if(isAlive(r,c-1)) n++;
    if(isAlive(r,c+1)) n++;
    if(isAlive(r+1,c-1)) n++;
    if(isAlive(r+1,c)) n++;
    if(isAlive(r+1,c+1)) n++;
    return n;
  }

  bool step()
  {
    for(const auto& r_it : alive_)
    {
      const auto r = r_it.first;
      for(auto& c : r_it.second)
      {
        auto n = countNeighbors(r,c);
        if(n==2 || n==3) next_alive_[r].insert(c);
      }
    }

    for(const auto& r_it : potential_newborn_)
    {
      const auto r = r_it.first;
      for(auto& c : r_it.second)
      {
	auto n = countNeighborsOfDead(r,c);
//        std::cout<<r<<"."<<c<<" -> "<<n<<std::endl;
        if(n==3) next_alive_[r].insert(c);
      }
    }
    alive_=next_alive_;
    next_alive_.clear();
    potential_newborn_.clear();
  }

protected:
  field_t alive_;
  field_t next_alive_;
  field_t potential_newborn_;
  const std::pair<uint, uint> output_size{50,50};
};

Game::Game(std::istream& in)
{
  while(in.good())
  {
    std::string r,c;
    std::getline(in,r,'.');
    std::getline(in,c,',');
    setLive(stoul(r),stoul(c));
  }
}

void Game::printField() const
{
  uint start_row=0;
  uint start_col=0;

  uint r = start_row;
  if(r==0)
  {
    for(uint c=0; c<output_size.second; c++) std::cout<<"-";
    std::cout<<std::endl;
  }

  for(; r<output_size.first; r++)
  {
    uint c = start_col;
    if(c==0)
    {
      std::cout<<"|";
    }

    for(start_row; c<output_size.second; c++)
    {
      std::cout<< (isAlive(r,c) ? "x" : " ");
    }

    if(c==output_size.second)
    {
      std::cout<<"|";
    }

    std::cout<<std::endl;
  }

  if(r==output_size.first)
  {
    for(uint c=0; c<output_size.second; c++) std::cout<<"-";
    std::cout<<std::endl;
  }

}

struct membuf : std::streambuf
{
    membuf(char* begin, char* end) {
        this->setg(begin, begin, end);
    }
};

int main(int argc, char** argv)
{
  if(argc<=1)
  {
    return 1;
  }

  char* buffer = argv[1];
  membuf sbuf(buffer, buffer + strlen(buffer));
  std::istream i(&sbuf);
  Game g(i);
  while(g.isAlive())
  {
    g.printField();
    g.step();
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  return 0;
}
