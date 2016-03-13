

#include <iostream>
#include <spot/tl/parse.hh>

// Parse a formula and print its atomic propositions.
int main (int argc, char ** argv)
{
  if (argc != 2)
    {
      std::cerr << "syntax: extractAP formula\n";
      return 1;
    }

  auto pf = spot::parse_infix_psl(argv[1]);
  if (pf.format_errors(std::cerr))
    return 1;

  pf.f.traverse([](spot::formula f)
		{
		  if (f.is(spot::op::ap))
		    std::cout << f.ap_name() << '\n';
		  return false;
		});

  return 0;
}
