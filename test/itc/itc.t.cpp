#include "itc/itc.hpp"
#include "testing.t.hpp"

namespace otto::itc {

  TEST_CASE ("ActionReciever") {
    using void_action = Action<struct void_action_tag>;
    using int_action = Action<struct int_action_tag, int>;

    struct VoidAR {
      void action(void_action)
      {
        has_run = true;
        run_count++;
      }
      bool has_run = false;
      int run_count = 0;
    };

    struct IntAR {
      void action(int_action, int val)
      {
        value += val;
      }
      int value = 0;
    };

    SECTION ("The ActionReciever concept") {
      struct EmptyStruct {};

      static_assert(ActionReciever::is<VoidAR, void_action>);
      static_assert(!ActionReciever::is<EmptyStruct, void_action>);
    }

    SECTION ("ActionData can be created using Action::data") {
      auto ad = int_action::data(10);
      static_assert(std::is_same_v<decltype(ad), ActionData<int_action>>);
      REQUIRE(std::get<0>(ad.args) == 10);
    }

    SECTION ("Action with no data can be recieved using call_reciever") {
      VoidAR void_ar;

      REQUIRE(void_ar.has_run == false);
      call_reciever(void_ar, void_action::data());
      REQUIRE(void_ar.has_run == true);
    }

    SECTION ("Action with an int argument can be recieved using call_reciever") {
      IntAR int_ar;

      call_reciever(int_ar, int_action::data(10));
      REQUIRE(int_ar.value == 10);
    }

    SECTION ("try_call_reciever calls action reciever and returns true") {
      IntAR int_ar;

      REQUIRE(try_call_reciever(int_ar, int_action::data(10)));
      REQUIRE(int_ar.value == 10);
    }

    SECTION ("try_call_reciever returns false when no reciever avaliable") {
      IntAR int_ar;
      REQUIRE(!try_call_reciever(int_ar, void_action::data()));
      REQUIRE(int_ar.value == 0);
    }

    SECTION ("ActionQueue") {
      ActionQueue aq;
      SECTION ("ActionQueue is initially empty") {
        REQUIRE(aq.size() == 0);
      }

      SECTION ("ActionQueue.push(function)") {
        bool has_run = false;
        aq.push([&] { has_run = true; });
        REQUIRE(aq.size() == 1);
        aq.pop()();
        REQUIRE(has_run);
      }

      SECTION ("ActionQueue.pop_call_all()") {
        IntAR iar;
        VoidAR var;
        aq.push(var, void_action::data());
        aq.push(iar, int_action::data(10));
        REQUIRE(aq.size() == 2);
        aq.pop_call_all();
        REQUIRE(var.has_run);
        REQUIRE(iar.value == 10);
      }

      SECTION ("ActionQueue.push(function) is FIFO ordered") {
        bool has_run = false;
        aq.push([&] { has_run = true; });
        aq.push([&] { REQUIRE(has_run); });
        REQUIRE(aq.size() == 2);
        aq.pop_call_all();
        REQUIRE(has_run);
      }

      SECTION ("ActionQueue.push(ActionReciever, ActionData) works") {
        VoidAR ar;
        aq.push(ar, void_action::data());
        aq.pop_call();
        REQUIRE(ar.has_run);
      }


      SECTION ("ActionQueue.try_push(ActionReciever, ActionData) works") {
        IntAR ar;
        aq.try_push(ar, int_action::data(10));
        aq.pop_call();
        REQUIRE(ar.value == 10);
      }

      SECTION ("ActionQueue.try_push(ActionReciever, ActionData) does not enqueue an action if the reciever cannot "
               "recieve it") {
        IntAR ar;
        REQUIRE(aq.try_push(ar, void_action::data()) == false);
        REQUIRE(aq.size() == 0);
      }
    }

    SECTION ("ActionSender") {
      struct OtherIntAR {
        void action(int_action, int val)
        {
          value += val;
        }
        int value = 0;
      } oiar;
      ActionQueue queue;
      VoidAR var;
      IntAR iar;
      ActionSender sndr = {queue, var, iar, oiar};

      SECTION ("ActionSender holds references to recievers") {
        REQUIRE(&sndr.reciever<VoidAR>() == &var);
        REQUIRE(&sndr.reciever<IntAR>() == &iar);
        REQUIRE(&sndr.reciever<OtherIntAR>() == &oiar);
      }

      SECTION ("ActionSender queues actions to all recievers") {
        sndr.push(int_action::data(10));
        REQUIRE(queue.size() == 2);
        queue.pop_call_all();
        REQUIRE(iar.value == 10);
        REQUIRE(oiar.value == 10);
      }

      SECTION ("ActionSender is copy constructible") {
        auto sndr2 = sndr;
        REQUIRE(&sndr.reciever<VoidAR>() == &var);
        REQUIRE(&sndr2.reciever<VoidAR>() == &var);
      }

      SECTION ("JoinedActionSender") {
        ActionQueue queue2;
        ActionSender sndr1 = {queue, var};
        ActionSender sndr2 = {queue2, iar, var};
        JoinedActionSender jsndr = {sndr1, sndr2};

        SECTION ("can push the same action to multiple queues") {
          jsndr.push(void_action::data());
          REQUIRE(queue.size() == 1);
          REQUIRE(queue2.size() == 1);
          queue.pop_call_all();
          REQUIRE(var.run_count == 1);
          queue2.pop_call_all();
          REQUIRE(var.run_count == 2);
        }
      }
    }

    SECTION ("Action with reference parameter") {
      using ref_act = Action<struct ref_act_tag, int&>;
      struct RefAR {
        void action(ref_act, int& ref)
        {
          addr = &ref;
        };
        int* addr = nullptr;
      };
      int an_int = 0;
      RefAR rar;
      call_reciever(rar, ref_act::data(an_int));
      REQUIRE(rar.addr == &an_int);
    }
  }

} // namespace otto::itc