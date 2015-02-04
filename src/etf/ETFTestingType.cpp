#include "ETFTestingType.hh"
#include <vector>
#include "Hom_Basic.hh"

using std::vector;

namespace its
{

  //Transition EtfTestingType::getAPredicate(Label predicate) const {
  //	// The predicate should respect the grammar : varName "=" .*
  //	// NB : NO whitespace allowed anywhere in the predicate string, or they will be parsed as part of identifiers.
  //	// Where varName is an instance name such as found in getVariableSet(), getVarOrder()
  //	// If the type of the variable is "byte" we expect to see an integer on the right.
  //	// If the type of the variable is some ad hoc type we expect to see the name of a value of that type on the right.
  //
  //
  //	AtomicProposition prop = (atomicPropLabel_to_AtomicProposition.find(predicate)->second).second;
  //	//int instindex = prop.var;
  //	//int value = prop.val;
  //	//std::cerr << " **********instindex: " << instindex << std::endl;
  //	//std::cerr << " **********value: " << value << std::endl;
  //	return localApply(varEqState(DEFAULT_VAR, prop.val), prop.var);
  //
  //}


  bdd
  EtfTestingType::state_condition(int state[]) const
  {
    bdd res = bddtrue;
    std::map<int, class AtomicProposition>::const_iterator it;
    for (it = bddvar_to_AtomicProposition.begin(); it
        != bddvar_to_AtomicProposition.end(); it++)
      {
        //std::cerr << " **********prop: " << it->first << std::endl;
        //std::cerr << " **********statevar: " << (it->second).first << std::endl;
        //std::cerr << " **********propvalue: " << (it->second).second
        //<< std::endl;
        AtomicProposition prop = (it->second);
        if (prop.comp(state[prop.var] - 1, prop.val))
          res &= bdd_ithvar(it->first);
        else
          res &= bdd_nithvar(it->first);
      }

    return res;
  }

  AtomicProposition
  EtfTestingType::parseAtomicProposition(Label predicate) const
  {
    // The predicate should respect the grammar : varName "=" .*
    // NB : NO whitespace allowed anywhere in the predicate string, or they will be parsed as part of identifiers.
    // Where varName is an instance name such as found in getVariableSet(), getVarOrder()
    // If the type of the variable is "byte" we expect to see an integer on the right.
    // If the type of the variable is some ad hoc type we expect to see the name of a value of that type on the right.


    // step 1 : parse the predicate
    const char * pred = predicate.c_str();
    int mode = 0;
    vLabel var, op, remain;
    for (const char * cp = pred; *cp; ++cp)
      {
        // skip ws
        if (*cp == ' ')
          continue;
        switch (mode)
          {
        // parsing variable
        case 0:
          if (*cp == '!' || *cp == '>' || *cp == '=' || *cp == '<')
            {
              mode = 1;
              op += *cp;
            }
          else
            {
              var += *cp;
            }
          break;
        case 1:
          if (*cp == '!' || *cp == '>' || *cp == '=' || *cp == '<')
            {
              op += *cp;
            }
          else
            {
              remain += *cp;
            }
          mode = 2;
          break;
        case 2:
          remain += *cp;
          }
      }

    std::tr1::function<bool
    (int, int)> comp = NULL;
    if (op == "=")
      {
        comp = std::equal_to<int>();
      }
    else if (op == "!=")
      {
        comp = std::not_equal_to<int>();
      }
    else if (op == ">")
      {
        comp = std::greater<int>();
      }
    else if (op == "<")
      {
        comp = std::less<int>();
      }
    else if (op == "<=")
      {
        comp = std::less_equal<int>();
      }
    else if (op == ">=")
      {
        comp = std::greater_equal<int>();
      }
    else
      {
        std::cerr << "Unrecognized comparison operator : " << op
            << " when parsing predicate " << predicate << std::endl;
        std::cerr << "Error is fatal, failing with error code 2" << std::endl;
        exit(2);
      }

    int N = lts_type_get_state_length(ltstype);
    int varindex = -1;
    for (int i = 0; i < N; i++)
      {
        vLabel var2 = lts_type_get_state_name(ltstype, i);
        if (var2 == var)
          {
            varindex = i;
            break;
          }
      }
    if (varindex == -1)
      {
        std::cerr
            << "Error variable " + var
                + " cannot be resolved as an instance name when trying to parse predicate : "
                + predicate << std::endl;
        std::cerr << "Expected one of : ";
        for (int i = 0; i < N; i++)
          {
            std::cerr << lts_type_get_state_name(ltstype, i) << ", ";
          }
        std::cerr << "\nFailing with error code 2" << std::endl;
        exit(2);
      }
    // This is the actual index given the current Ordering
    int instindex = getVarOrder()->getIndex(var);

    // Find the type
    vLabel type = lts_type_get_state_type(ltstype, varindex);
    if (type == "byte" || type == "int")
      {
        int value = 0;
        if (sscanf(remain.c_str(), "%d", &value) == 0)
          {
            std::cerr
                << "Unable to parse right hand side of comparison as an integer : "
                << remain << " when parsing predicate " << predicate
                << std::endl;
            std::cerr << "Error is fatal, failing with error code 2"
                << std::endl;
            exit(2);
          }
        //std::cerr << "Ok for predicate : var" << instindex << op << value
        //	<< std::endl;
        return AtomicProposition(instindex, comp, value);
      }
    else
      {
        int type_count = lts_type_get_type_count(ltstype);
        for (int i = 0; i < type_count; i++)
          {
            //      Warning(info,"dumping type %s",lts_type_get_type(ltstype,i));
            vLabel type2 = lts_type_get_type(ltstype, i);
            //      std::cerr << "testing type " << type2 <<" against "<< type << std::endl;
            if (type2 == type)
              {
                // Ids are quoted in LTSmin
                remain = "\"" + remain + "\"";
                // Found the right type
                int values = etf_get_value_count(etfmodel, i);
                for (int j = 0; j < values; j++)
                  {
                    chunk c = etf_get_value(etfmodel, i, j);
                    size_t len = c.len * 2 + 3;
                    char str[len];
                    chunk2string(c, len, str);
                    //std::cerr << "testing value " << str << " against "
                    //<< remain << std::endl;

                    if (vLabel(str) == remain)
                      {
                        // Hit !
                        //std::cerr << "Ok for predicate : var" << instindex
                        //	<< op << j << std::endl;
                        return AtomicProposition(instindex, comp, j);

                      }
                  }
                  {
                    std::cerr << "Unable to find value \"" << remain
                        << "\" in type " << type << " of variable " << var;
                    std::cerr << " when parsing predicate " << predicate
                        << std::endl;
                    std::cerr << "Expected a value among :";
                    int values = etf_get_value_count(etfmodel, i);
                    for (int j = 0; j < values; j++)
                      {
                        chunk c = etf_get_value(etfmodel, i, j);
                        size_t len = c.len * 2 + 3;
                        char str[len];
                        chunk2string(c, len, str);
                        std::cerr << str << "  ;";
                      }

                    std::cerr << std::endl
                        << "Error is fatal, failing with error code 2"
                        << std::endl;
                    exit(2);
                  }
              }
          }
          {
            std::cerr << "Unable to find type \"" << type
                << "\" in type declarations of model ";
            std::cerr << " when parsing predicate " << predicate << std::endl;
            std::cerr << "Error is fatal, failing with error code 2"
                << std::endl;
            exit(2);
          }

      }
    // to please the compiler
    return AtomicProposition();
  }

  void
  EtfTestingType::setObservedAP(Label atomicProp, int bddvar)
  {
    //std::cerr << " setObservedAP " << std::endl;
    bddvar_to_AtomicProposition[bddvar] = parseAtomicProposition(atomicProp);
  }

  void
  EtfTestingType::compute_transitions_by_changesets(std::set<bdd,
      spot::bdd_less_than> allChangesets)
  {

    //std::cerr << " compute transitions by changeset: " << changeset
    //<< std::endl;
    int N = lts_type_get_state_length(ltstype);

    //std::cerr << "state_length: " << N << std::endl;

    int nbevents = etf_trans_section_count(etfmodel);
    //std::cerr << "nbevents: " << nbevents << std::endl;
    //printf("N and nbevents %4d/%d\n",N,nbevents);

    // Initialize the relation
    //std::cerr << "Initialize the relation" << std::endl;
    for (int section = 0; section < nbevents; section++)
      {

        //printf("group %4d/%d\n", section + 1, nbevents);

        etf_rel_t trans = etf_trans_section(etfmodel, section);

        int src[N];
        int dst[N];
        int lbl[nbevents];

        vector<int> proj;

        ETFrelIterate(trans);
        if (ETFrelNext(trans, src, dst, lbl))
          {
            for (int i = 0; i < N; i++)
              {
                if (src[i])
                  {
                    proj.push_back(i);
                  }
              }
          }
        int len = proj.size();

        Sgi::hash_map<int, ETFTransition, Sgi::hash<int> > bdd_to_ETFtransition;

        ETFrelIterate(trans);
        while (ETFrelNext(trans, src, dst, lbl))
          {
            bdd changeset = bdd_setxor(state_condition(src), state_condition(
                dst));
            //std::cerr << " ******** changeset between src and dst: " << changeset << std::endl;
            std::set<bdd, spot::bdd_less_than>::iterator it;
            for (it = allChangesets.begin(); it != allChangesets.end(); ++it)
              {
                bdd dont_care_changeset = *it;

                if (bdd_implies(changeset, dont_care_changeset))
                  {
                    Sgi::hash_map<int, ETFTransition, Sgi::hash<int> >::iterator
                        iteratorETFtransition = bdd_to_ETFtransition.find(
                            dont_care_changeset.id());

                    if (iteratorETFtransition == bdd_to_ETFtransition.end())
                      {
                        iteratorETFtransition
                            = (bdd_to_ETFtransition.insert(std::pair<int,
                                ETFTransition>(dont_care_changeset.id(),
                                ETFTransition(proj)))).first;
                      }

                    int psrc[len];
                    int pdst[len];

                    for (int ii = 0; ii < len; ++ii)
                      {
                        psrc[ii] = src[proj[ii]] - 1;
                        pdst[ii] = dst[proj[ii]] - 1;
                      }

                    (iteratorETFtransition->second).addEdge(psrc, pdst);
                  }

              }

          }

        Sgi::hash_map<int, ETFTransition, Sgi::hash<int> >::const_iterator i =
            bdd_to_ETFtransition.begin();
        while (i != bdd_to_ETFtransition.end())
          {
            changeset_to_transitions[i->first].push_back(i->second);
            ++i;
          }

      }

  }

  /** state and transitions representation functions, selected by changeset */
  /** Local transitions by changeset*/
  Transition
  EtfTestingType::getLocalsByChangeSet(bdd changeset) const
  {

    vector<ETFTransition> transitions;
    Sgi::hash_map<int, vector<ETFTransition> , Sgi::hash<int> >::const_iterator
        i = changeset_to_transitions.find(changeset.id());

    if (i != changeset_to_transitions.end())
      {
        transitions = i->second;
      }
    else
      {
        return Transition::null;
      }

    //compute_transitions_by_changeset(transitions, changeset);

    d3::set<GShom>::type toadd;
    for (vector<ETFTransition>::const_iterator it = transitions.begin(); it
        != transitions.end(); ++it)
      {
        toadd.insert(it->getShom());
      }

    return GShom::add(toadd);
  }

//EtfTestingType::~EtfTestingType() {
//	if (dict)
//		dict->unregister_all_my_variables(this);
//}

//void EtfTestingType::compute_transitions_by_changeset(
//		vector<ETFTransition> & transitions, bdd changeset) const {
//
//	int N = lts_type_get_state_length(ltstype);
//
//	int nbevents = etf_trans_section_count(etfmodel);
//	// Initialize the relation
//	for (int section = 0; section < nbevents; section++) {
//
//		//printf("group %4d/%d\n", section + 1, nbevents);
//
//		etf_rel_t trans = etf_trans_section(etfmodel, section);
//
//		int src[N];
//		int dst[N];
//		int lbl[nbevents];
//
//		vector<int> proj;
//
//		bdd changeset_src_dst = bdd_xor(state_condition(src), state_condition(
//				dst));
//
//		ETFrelIterate(trans);
//		if (ETFrelNext(trans, src, dst, lbl)) {
//			for (int i = 0; i < N; i++) {
//				if (src[i]) {
//					proj.push_back(i);
//				}
//			}
//		}
//		int len = proj.size();
//
//		transitions.push_back(ETFTransition(proj));
//
//		ETFrelIterate(trans);
//		while (ETFrelNext(trans, src, dst, lbl)) {
//			int psrc[len];
//			int pdst[len];
//
//			for (int ii = 0; ii < len; ++ii) {
//				psrc[ii] = src[proj[ii]] - 1;
//				pdst[ii] = dst[proj[ii]] - 1;
//			}
//			bdd changeset_src_dst = bdd_setxor(state_condition(src),
//					state_condition(dst));
//			if (changeset_src_dst == changeset)
//				transitions[section].addEdge(psrc, pdst);
//		}
//	}
//
//}

}
