#include "llvm/Pass.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/InitializePasses.h"
#include "llvm/Transforms/Scalar.h"
#include <vector>

#define DEBUG_TYPE "func-block-count"

using namespace llvm;
using namespace std;

namespace {
    struct FunctionBlockCount : public FunctionPass {
        static char ID;
        FunctionBlockCount() : FunctionPass(ID) {
            initializeFunctionBlockCountPass(*PassRegistry::getPassRegistry());
        }

        void countBlocksInLoop(Loop *L, unsigned int nest) {
            unsigned int numBlocks = 0;
            Loop::block_iterator bb;
            for (bb = L->block_begin(); bb != L->block_end(); ++bb) {
                ++numBlocks;
            }
            errs() << "Loop level " << nest << " has " << numBlocks << " blocks\n";
            std::vector<Loop*> subLoops = L->getSubLoops();
            Loop::iterator j, f;
            for (j = subLoops.begin(), f = subLoops.end(); j!=f; ++j)
                countBlocksInLoop(*j, nest+1);
            /*
            for (unsigned int i = 0; i < subLoops.size(); ++i) {
                countBlocksInLoop(subLoops[i], nest+1);
            }
            */
        }

        bool runOnFunction(Function &F) override {
            LoopInfo *LI = &getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
            errs() << "Function " << F.getName() << '\n';
            for (Loop *L : *LI) {
                countBlocksInLoop(L, 0);
            }
            return false;
        }

        // This example modifies the program, but does not modify the CFG
        void getAnalysisUsage(AnalysisUsage &AU) const {
            //AU.setPreservesCFG();
            AU.addRequired<LoopInfoWrapperPass>();
        }
    };
}

char FunctionBlockCount::ID;
INITIALIZE_PASS_BEGIN(FunctionBlockCount, "functionblockcount", "Function Block Count", false, false)
INITIALIZE_PASS_DEPENDENCY(LoopInfoWrapperPass)
INITIALIZE_PASS_END(FunctionBlockCount, "functionblockcount", "Function Block Count", false, false)

FunctionPass *llvm::createFunctionBlockCountPass() { return new FunctionBlockCount(); }
