/*
* MarkovChain.h -> class for defining the headers of MarkovChain's functions
* 
*/


#pragma once

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <random>


//Definition of the variables
typedef std::vector<std::string> state_sequence;
typedef std::string state_single;
typedef std::pair<state_single, state_single> state_and_observation;

class MarkovChain {
public:
	//Constructor
	//Takes as opcional argument _maxOrder
	MarkovChain(unsigned long _maxOrder = 2);
	~MarkovChain();

	//Definition of function for MarkovChain

	void addObservation(const state_sequence& prevState, state_single currentState);
	/*
	* addObservation -> adds an observation to the chain
	* Takes the previous states withing a sequence (state_sequence) which calls prevState
	* The current state observed is currentState
	*/

	void addObservationAllOrders(const state_sequence& prevState, state_single currentState);
	/*
	* addObservationAllOrders -> adds an observation to multiple different orders in a Markov chain model.
	* Starts breaking down the prevState into all possible orders, i.e. divides the sequence of previous states
	* into subsequences of varying lenghts, ranging from the highest order to the lower.
	* Then, it iterates through all the generated subsequences from the previous step. 
	* For each iteration, calls "addObservation" to add "currentState" observation to the model.
	* Finally, the model is able to consider transition btw different orders, generating more realistic sequences.
	* 
	*/

	std::vector<state_sequence> breakStateIntoAllOrders(const state_sequence& prevState);
	/*
	* 
	*/

	std::string stateSequenceToString(const state_sequence& sequence);
	/*
	* 
	*/

	std::string stateSequenceToString(const state_sequence& sequence, long unsigned int maxOrder);

	/*
	* 
	*/

	state_single generateObservation(const state_sequence& prevState, int maxOrderWanted, bool needChoice = false);
	/*
	* Important method!!
	* Generates an observation based on the prevState and its order. If it does not find anything, it'll reduce 
	* the order until it returns something
	* prevState -> sequences of previous states in the model
	* maxOrderWanted -> order of the mrkovmodel
	* needChoice -> if set to true, it'll try to return at least two choices 
	* return: a state sampled from the model
	* 
	*/

	state_single pickRandomObservation(const state_sequence& seq);
	/*
	* Picks a random observation from the sequence
	*/

	std::string toString();
	/*
	* 
	*/

	bool fromString(const std::string& savedModel);
	/*
	* 
	*/

	void reset();
	/*
	* Reset the chain
	*/

	int getOrderOfLastMatch();
	/*
	* 
	*/

	state_single zeroOrderSample();
	/*
	* 
	*/

	state_and_observation getLastMatch();
	/*
	* 
	*/

	void removeMapping(state_single state_key, state_single unwanted_option);
	/*
	* 
	*/

	void amplifyMapping(state_single state_key, state_single unwanted_option);
	/*
	* 
	*/

	long size();
	/*
	* 
	*/

	bool validateStateSequence(const state_sequence& seq);
	/*
	* 
	*/

	static std::vector<std::string> tokenise(const std::string& s, char separator);
	/*
	* 
	*/


private:

	state_sequence getOptionsForSequenceKey(state_single seqAsKey);
	/*
	* Returns the available states that follows the sent key
	*/

	static bool validateStateToObservationsString(const std::string& s);
	/*
	* Checks if the string is suitable after becoming a string
	*/

	std::map<state_single, state_sequence> model;
	unsigned long maxOrder;
	unsigned long orderOfLastMatch;
	state_and_observation lastMatch;
	/*
	* maps from string keys to possible next states(basically what joins eveything and creates the model)
	*/

};