#include "fsltltesting.hh"
#include "ddd/statistic.hpp"

//#define trace std::cerr
#define trace while (0) std::cerr

namespace its
{

  // Create a type to hold a circular set.
  bool
  fsltlTestingModel::declareETFType(Label path)
  {
    trace
      << "fsltlModel::declareETFType with path=" << path << std::endl;
    model_labled_by_changesets_ = new its::EtfTestingType(path);

    //addType of model_labled_by_changesets_
    pType oldtype = findType(model_labled_by_changesets_->getName());
    if (oldtype != NULL)
      {
        std::cerr << " ITS type " << model_labled_by_changesets_->getName()
            << " already exists in model.\n";
        delete model_labled_by_changesets_;
        return false;
      }
    // Add model_labled_by_changesets_
    types_.push_back(model_labled_by_changesets_);
    return true;
  }

  bool
  fsltlTestingModel::buildComposedSystem()
  {
    // instance not set
    if (getInstance() == NULL)
      {
        std::cerr
            << "Main instance state not set when calling \"buildComposedSystemTGTA\"\n";
        return false;
      }

    // init state unset
    if (getInitialState() == State::null)
      {
        std::cerr
            << "Initial state not set when calling \"buildComposedSystemTGTA\"\n";
        return false;
      }

    // lookup the TGBA
    if (!tgba_)
      {
        std::cerr << "TGTA not set when calling \"buildComposedSystemTGTA\"\n";
        return false;
      }

    // Verbose print of built system
    // trace << *this << std::endl;

    // Ok preconditions fulfilled.

    // Grab the TGTA labels
    labels_t tgbalabs = tgba_->getTransLabels();

    std::set<bdd, spot::bdd_less_than> all_changesets;

    for (labels_it it = tgbalabs.begin(); it != tgbalabs.end(); ++it)
      {
        TgbaType::tgba_arc_label_t arcLab =
            tgba_->getTransLabelDescription(*it);

        // in TGTA, change_set = (source_state_condition) XOR (destination_state_condition)
        bdd change_set = arcLab.first;
        all_changesets.insert(change_set);
      }

    //Statistic sB(initState_, "*** Before compute_transitions_by_changesets");
    //sB.print_line(std::cerr);
    model_labled_by_changesets_->compute_transitions_by_changesets(
        all_changesets);

    //Statistic sA(initState_, "*** After compute_transitions_by_changesets");
    //	sA.print_line(std::cerr);

    for (labels_it it = tgbalabs.begin(); it != tgbalabs.end(); ++it)
      {
        trace
          << "label :" << *it << std::endl;
        TgbaType::tgba_arc_label_t arcLab =
            tgba_->getTransLabelDescription(*it);
        trace
          << "desc :" << arcLab.first << ":" << arcLab.second << std::endl;

        labels_t labtodo;
        labtodo.push_back(*it);

        // in TGTA, change_set = (source_state_condition) XOR (destination_state_condition)
        bdd change_set = arcLab.first;

        Transition model_transition =
            model_labled_by_changesets_->getLocalsByChangeSet(change_set);

        Transition toadd = localApply(tgba_->getSuccs(labtodo), 1)
            & localApply(model_transition, 0);

        if ((bdd_setxor(change_set, change_set) == change_set))
          allStutteringTrans_ = allStutteringTrans_ + (localApply(
              Transition::id, 1) & localApply(model_transition, 0));
        else
          allTrans_ = allTrans_ + toadd;

        labels_t accs = tgba_->getAcceptanceSet(arcLab.second);
        for (labels_it acc = accs.begin(); acc != accs.end(); ++acc)
          {
            accToTrans_it accit = accToTrans_.find(*acc);
            if (accit == accToTrans_.end())
              {
                // first occurrence
                accToTrans_[*acc] = toadd;
              }
            else
              {
                accit->second = accit->second + toadd;
              }
          }
      }
    allTrans_ = allTrans_ + allStutteringTrans_;
    return true;
  }

  State
  fsltlTestingModel::getInitState()
  {
    //Statistic s(initState_, "before initState");
    //			s.print_line(std::cerr);

    if (initState_ == State::null)
      {
        Transition allTrans = Transition::null;
        labels_t tgbalabs = tgba_->getTransLabels();
        for (labels_it it = tgbalabs.begin(); it != tgbalabs.end(); ++it)
          {
            trace
              << "label :" << *it << std::endl;
            TgbaType::tgba_arc_label_t arcLab =
                tgba_->getTransLabelDescription(*it);
            trace
              << "desc :" << arcLab.first << ":" << arcLab.second << std::endl;
            Transition apcond = sogIts_->getSelector(arcLab.first,
                getInstance()->getType());

            labels_t labtodo;
            labtodo.push_back(*it);
            Transition toadd = localApply(tgba_->getSuccs(labtodo), 1)
                & localApply(apcond, 0);

            allTrans = allTrans + toadd;
          }

        initState_ = allTrans(State(1, findType("TGBA")->getState("init"),
            State(0, getInstance()->getType()->getState("init"))));

      }

    return initState_;
  }

}

