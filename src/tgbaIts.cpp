#include "tgbaIts.hh"
#include "tgbaalgos/reachiter.hh"


namespace its {

  const std::string TgbaType::name_ = "TGBA";


  namespace loadTGBA
  {
    
    class load_bfs: public spot::tgba_reachable_iterator_breadth_first
    {
      TgbaType::arcs_t & arcs_;
    public:
      load_bfs(const spot::tgba* a, TgbaType::arcs_t & arcs)
	: tgba_reachable_iterator_breadth_first(a),arcs_(arcs)
      {
      }

      /// Called by run() to process a state.
      ///
      /// \param s The current state.
      /// \param n A unique number assigned to \a s.
      /// \param si The spot::tgba_succ_iterator for \a s.
      void process_state(const spot::state* s, int n,spot:: tgba_succ_iterator* si)
      {
      }

      /// Called by run() to process a transition.
      ///
      /// \param in_s The source state
      /// \param in The source state number.
      /// \param out_s The destination state
      /// \param out The destination state number.
      /// \param si The spot::tgba_succ_iterator positionned on the current
      ///             transition.
      ///
      /// The in_s and out_s states are owned by the
      /// spot::tgba_reachable_iterator instance and destroyed when the
      /// instance is destroyed.
      void process_link(const spot::state* in_s, int in,
			const spot::state* out_s, int out,
			const spot::tgba_succ_iterator* si) 
      {
	TgbaType::tgba_arc_label_t lab (si->current_condition(), si->current_acceptance_conditions());
	TgbaType::tgba_arc_t arc (in,out);
	TgbaType::arcs_it it = arcs_.find(lab);
	if ( it != arcs_.end() ) {
	  // already some arcs matched to this key
	  it->second.push_back(arc);
	} else {
	  arcs_.insert(TgbaType::arcs_t::value_type(lab, TgbaType::tgba_arcs_t(1,arc)));
	}
      }
    };
  }

  TgbaType::TgbaType (const spot::tgba * tgba) : tgba_(tgba) 
  {
    loadTGBA::load_bfs b(tgba, arcs_);    
    b.run();
  }

}
