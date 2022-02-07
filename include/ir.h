#pragma once
#include <string>
#include <vector>
#include <memory>
#include <map>
/**
 * yapyjit uses a linear IR based on var-len instructions as a layer
 * between python AST and backend (MIR currently).
 */

namespace yapyjit {
	// Base class for all instructions
	class Instruction {
	public:
		virtual ~Instruction() = default;
	};

	// Empty nop instruction designated for use as labels
	class LabelIns: public Instruction {
	};

	class ReturnIns : public Instruction {
	public:
		int local_id;
		ReturnIns(int _local_id) : local_id(_local_id) {}
	};

	class Function {
	public:
		std::string name;
		std::vector<std::unique_ptr<Instruction>> instructions;
		std::map<std::string, int> locals;
		Function(std::string _name) : name(_name) {}
	};
};
