/** Implementation file for table craps simulation
 *
 * \file imgs/apps/examples/craps/craps.cpp
 * \author Josh Carstens, proponent of cran-grape juice (jdc3498@rit.edu)
 * \date 10 April 2020
 * \note Name me a better juice than cran-grape juice
 */

#include <algorithm>
#include <chrono>
#include <random>
#include <iostream>
#include <string>
#include <vector>

#include <boost/program_options.hpp>

using namespace std;
namespace po = boost::program_options;

void roll_dice(std::vector<int>& dice, int& roll) {
  // using the ol Mersenne Twister for extra entropy
  mt19937 rand(chrono::steady_clock::now().time_since_epoch().count());
  uniform_int_distribution<int> distribution(1, 6);
  dice[0] = distribution(rand);
  dice[1] = distribution(rand);
  roll = dice[0] + dice[1];
}

int main(int argc, char* argv[]) {
  // Flag indicating if the program should be run in interactive mode
  // (i.e. requiring a <RET> to be hit between each game)
  bool interactive;

  // Flag indicating that a "Pass" bet should be used as the "line" bet
  // rather than the default "Don't Pass Bar 12" bet
  bool pass;

  // Flag indicating whether the Martingale betting system should be employed
  bool use_martingale_system;

  // Flag indicating whether an "odds" bet should be placed along with the
  // "line" bet (for sessions when the Martingale betting system is
  // being used as well as when it is not)
  bool odds;

  // Number of session simulations to be carried out (i.e. where a session
  // is defined as playing games until desired winnings are achieved -or-
  // the bankroll is exhausted)
  int number_of_simulations;

  // The minimum bet to be placed (i.e. the initial "line" bet as well as
  // the bet to be placd after any winning game -or- as a stand-alone "odds"
  // bet)
  int minimum_bet_amount;

  // The bankroll that play is started with
  int initial_bankroll;

  // Desired winnings, which if achieved, would indicate that session play
  // should be terminated
  int walk_away_winnings;

  // **************************************************************************
  // Implement command-line interface utilizing Boost program options library
  // **************************************************************************
  // Instantiate the "visible options" object that defines the program options
  // that are to be visible to the user
  po::options_description visible_options("Options");
  visible_options.add_options()("help,h", "display this message")(
      "interactive,i", po::bool_switch(&interactive)->default_value(false),
      "interactive mode (require <RET> after each game in a session) "
      "[default is false]")(
      "number-of-simulations,n",
      po::value<int>(&number_of_simulations)->default_value(1),
      "number of 'walk-away-winnings' session simulations to carry out "
      "[default is 1]")(
      "pass,p", po::bool_switch(&pass)->default_value(false),
      "place a Pass bet [default is false] (i.e. place a Don't Pass Bar "
      "12 bet)")("martingale,m",
                 po::bool_switch(&use_martingale_system)->default_value(false),
                 "use the Martingale betting system [default is false]")(
      "odds,o", po::bool_switch(&odds)->default_value(false),
      "place an Odds bet [default is false]");

  // Instantiate the "hidden options" object that defines the program options
  // that are to be hidden from the user -or- that are to be used as
  // positional arguments
  po::options_description hidden_options("Hidden Options");
  hidden_options.add_options()("bet,b", po::value<int>(&minimum_bet_amount),
                               "minimum bet amount")(
      "bankroll,r", po::value<int>(&initial_bankroll), "initial bankroll")(
      "walk-away-winnings,w", po::value<int>(&walk_away_winnings),
      "winnings after which you'll walk away from the table");

  // Instantiate the "all options" object (combining the "visible options"
  // and "hidden options" objects)
  po::options_description all_options("All Options");
  all_options.add(visible_options).add(hidden_options);

  // Instantiate the "positional options" object, defining the REQUIRED
  // positional command-line parameters that must be provided by the user,
  // in order, as the last elements on the command line (these parameters
  // must also be defined above in the "hidden options" object)
  po::positional_options_description positional_options;
  positional_options.add("bet", 1);
  positional_options.add("bankroll", 1);
  positional_options.add("walk-away-winnings", 1);

  // Instantiate the Boost program options variable map (command line to
  // variable mapping definition)
  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv)
                .options(all_options)
                .positional(positional_options)
                .run(),
            vm);
  po::notify(vm);

  // Display usage message to the standard output if the --help or -h
  // optional parameter is specified by the user (positional argument
  // names must be specified here explicitly)
  if (vm.count("help")) {
    cout << "Usage: " << argv[0] << " [options] bet bankroll walk-away-winnings"
         << endl;
    cout << visible_options << endl;
    return EXIT_SUCCESS;
  }

  // *** ERROR CHECK ***
  // Check that multiple simulations and interactive mode are not specified
  // together (warn and exit if detected)
  if (number_of_simulations > 1 && interactive) {
    cout << "Multiple simulations and interactive mode not permitted" << endl;
    cout << endl;
    return EXIT_FAILURE;
  }

  // *** ERROR CHECK ***
  // Check if the requested number of simulations is less than or equal to 0
  // (warn and exit if detected)
  if (number_of_simulations < 1) {
    cout << "Number of simulations must be greater than or equal to 1" << endl;
    cout << endl;
    return EXIT_FAILURE;
  }

  // If the number of simulations is greater than 1, silence the per
  // game/session reporting dialog (setting the verbose flag to false)
  bool verbose = (number_of_simulations == 1) ? true : false;
  //bool verbose = true;

  // Display the run-time parameters for the simulation
  cout << "----------------------------------------"
       << "----------------------------------------" << endl;
  cout << "Bet type: " << (pass ? "Pass" : "Don't Pass Bar 12") << endl;
  cout << "Place odds bet: " << (odds ? "Yes" : "No") << endl;
  cout << "Use Martingale betting system: "
       << (use_martingale_system ? "Yes" : "No") << endl;
  cout << "Interactive mode: " << (interactive ? "On" : "Off") << endl;
  cout << "Verbosity: " << (verbose ? "On" : "Off") << endl;
  cout << "Number of simulated sessions to perform: " << number_of_simulations
       << endl;
  cout << "Minimum bet: $" << minimum_bet_amount << endl;
  cout << "Initial bankroll: $" << initial_bankroll << endl;
  cout << "Winnings after which you will walk away: $" << walk_away_winnings
       << endl;
  cout << "----------------------------------------"
       << "----------------------------------------" << endl;
  cout << endl;

  // Set initial dice elements
  vector<int> dice(2);
  int roll = 0;

  // Some values to store statistical information
  double num_games = 0;
  double num_games_won = 0;
  double num_walks = 0;

  // Permanent store of our bet amount
  auto init_bet_amount = minimum_bet_amount;

  // "Actual" bet amount
  auto bet_amount = init_bet_amount;

  // Value that tracks bankroll over multiple games
  auto bankroll = initial_bankroll;

  // Repeats the simulation if number_of_simulations > 1
  for (int idx = 0; idx < number_of_simulations; idx++) {
    // Session loop, ends if the desired earnings are met or money is depleted
    while (bankroll < (initial_bankroll + walk_away_winnings) && bankroll > 0) {
      // Used to compare the previous and current bankroll value to detect loss
      auto prev_bankroll = bankroll;

      // The come-out roll
      roll_dice(dice, roll);

      // All my couts from here on will be wrapped in if statements to account
      // for the verbose flag (which was a pain), let me know if there's a more
      // efficient way to do this
      if (verbose == true) {
        cout << "Desired line bet: $" << minimum_bet_amount << endl;
        cout << "Line bet: $" << bet_amount << endl;
        cout << "Come out roll: " << dice[0] << " + " << dice[1] << " = "
             << roll << endl;
      }

      // Come-out roll conditions
      if (roll == 7 || roll == 11) {
        if (pass == false) {
          bankroll -= bet_amount;
          if (verbose == true) {
            cout << "RESULT: You rolled a " << roll
                 << " on the come-out roll, you lose :(" << endl;
          }
        } else {
          bankroll += bet_amount;
          num_games_won++;  // We count every time a game is won for statistics
          if (verbose == true) {
            cout << "RESULT: You rolled a " << roll
                 << " on the come-out roll, you win :)" << endl;
          }
        }
      } else if (roll == 2 || roll == 3 || roll == 12) {
        if (pass == false) {
          if (roll != 12) {
            bankroll += bet_amount;
            num_games_won++;
            if (verbose == true) {
              cout << "RESULT: You rolled a " << roll
                   << " on the come-out roll, you win :)" << endl;
            }
          } else {
            if (verbose == true) {
              cout << "RESULT: You rolled a " << roll
                   << " on the come-out roll, it's a tie :|" << endl;
            }
          }
        } else {
          bankroll -= bet_amount;
          if (verbose == true) {
            cout << "RESULT: You rolled a " << roll
                 << " on the come-out roll, you lose :(" << endl;
          }
        }
      } else {
        // The point is established
        auto point = roll;
        if (verbose == true) {
          cout << "Point: " << point << endl;
        }

        // Keep rolling until a 7 or the point rolled
        // At first I had "roll != 7 && roll != point" as the condition for this
        // loop and then I rolled the dice once before entering it, but then I
        // had to immediately roll the dice again before printing it. Doing it
        // this way allows for accurate dice reporting and tracking
        while (true) {
          roll_dice(dice, roll);  // First we roll the dice
          if (verbose == true) {  // Then we print it
            cout << "Roll: " << dice[0] << " + " << dice[1] << " = " << roll
                 << endl;
            if (roll == 7 || roll == point) {  // Then we check if it's a 7 or
                                               // the point and break if it is
              break;
            }
          }
        }

        // Conditions when a 7 is rolled
        if (roll == 7) {
          if (pass == false) {
            bankroll += bet_amount;
            num_games_won++;
            if (verbose == true) {
              cout << "RESULT: You rolled a " << roll
                   << " before the point, you win :)" << endl;
            }
          } else {
            bankroll -= bet_amount;
            if (verbose == true) {
              cout << "RESULT: You rolled a " << roll
                   << " before the point, you lose :(" << endl;
            }
          }

          // Conditions if the point is rolled
        } else {
          if (pass == false) {
            bankroll -= bet_amount;
            if (verbose == true) {
              cout << "RESULT: You rolled the point, you lose :(" << endl;
            }
          } else {
            bankroll += bet_amount;
            num_games_won++;
            if (verbose == true) {
              cout << "RESULT: You rolled the point, you win :)" << endl;
            }
          }
        }
      }

      // Printing the current bankroll
      if (verbose == true) {
        cout << "Bankroll: $" << bankroll << endl;
      }

      // Misc. post-game accounting and implementation

      num_games++;  // Tracking the total number of games

      // Martingale System implementation
      if (use_martingale_system == true) {
        if (bankroll < prev_bankroll) {  // If we lost money
          minimum_bet_amount *= 2;       // Double our next bet
        } else {                         // If we haven't lost money
          minimum_bet_amount =
              init_bet_amount;  // Reset our next bet to our specified value
        }
        bet_amount = minimum_bet_amount;  // Update the actual bet amount
      }

      // Handles when there isn't enough money remaining to meet the desired
      // line bet
      if (bankroll < bet_amount) {
        bet_amount = bankroll;  // If there isn't enough to meet the minimum
                                // bet, go all-in
      } else {
        bet_amount = init_bet_amount;  // Reset to the specified amount
      }

      // Interactive mode implementation
      if (interactive == true) {
        cin.ignore(std::numeric_limits<streamsize>::max(), '\n');
      } else {
        if (verbose == true) {
          cout << endl;
        }
      }
    }

    // Misc. post-simulation accounting and implementation

    if (bankroll > 0) {
      num_walks++;  // Add to the count of simulations where we satisfy
                    // walk_away_winnings
    }

    // Resetting our bankroll and bet amounts going into the next simulation
    bankroll = initial_bankroll;
    bet_amount = init_bet_amount;
    minimum_bet_amount = init_bet_amount;
  }
  // Statistics outputs
  if (number_of_simulations > 1) {
    cout << "The overall probability of winning (percentage of 'games' won): "
         << (num_games_won / num_games) << "%" << endl;
    cout << "Average number of 'games' played per simulation: "
         << (num_games / number_of_simulations) << endl;
    cout << "Simulations you successfully walked away from: " << num_walks
         << " out of " << number_of_simulations << " ("
         << (num_walks / number_of_simulations) << "%)" << endl;
  }
  return EXIT_SUCCESS;
}
