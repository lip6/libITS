#ifndef FORCE_HH_
#define FORCE_HH_

#include <string>
#include <map>
#include <set>

typedef std::pair<std::string, std::string> edge;
typedef std::map<std::string, int> order;

order force (const std::set<edge> &, const order &);

#endif
