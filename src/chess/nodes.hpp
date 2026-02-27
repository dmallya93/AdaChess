// ---------------------------------------------------------------------------
// AdaChess – Node type definitions
// Translated from: chess-nodes.ads
// ---------------------------------------------------------------------------
#pragma once

#include <cstdint>

namespace chess {

// -----------------------------------------------------------------------
// Node_Type: unsigned 64-bit counter for search nodes.
// Ada: type Node_Type is mod 2 ** 64 with Size => 64;
// -----------------------------------------------------------------------
using NodeCount = std::uint64_t;

// -----------------------------------------------------------------------
// Perft_Node_Type: limited range for perft results (up to perft 10).
// Ada: subtype Perft_Node_Type is Node_Type range 0 .. 2 ** 46;
// -----------------------------------------------------------------------
using PerftNodeCount = std::uint64_t;

inline constexpr PerftNodeCount Max_Perft_Nodes = std::uint64_t{1} << 46;

// -----------------------------------------------------------------------
// Tree_Node_Type: PV, Cut, or All node classification for alpha-beta.
// Ada: type Tree_Node_Type is (Pv_Node, Cut_Node, All_Node)
//        with Default_Value => Pv_Node, Size => 2;
// -----------------------------------------------------------------------
enum class TreeNode : std::uint8_t {
    Pv_Node  = 0,
    Cut_Node = 1,
    All_Node = 2
};

/// Flip node type — corresponds to Ada's "not" operator on Tree_Node_Type.
/// Pv_Node => Pv_Node, Cut_Node => All_Node, All_Node => Cut_Node.
[[nodiscard]] constexpr TreeNode operator~(TreeNode node) noexcept {
    switch (node) {
        case TreeNode::Pv_Node:  return TreeNode::Pv_Node;
        case TreeNode::Cut_Node: return TreeNode::All_Node;
        case TreeNode::All_Node: return TreeNode::Cut_Node;
    }
    return TreeNode::Pv_Node; // unreachable
}

} // namespace chess
