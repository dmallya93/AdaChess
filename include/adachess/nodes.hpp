/**
 * AdaChess - Smart Chess Engine
 *
 * Node type definitions for search tree statistics.
 * C++ port of the original Ada implementation (chess-nodes.ads).
 *
 * Original Copyright (C) 2013-2023 - Alessandro Iavicoli
 * C++ Port Copyright (C) AdaChess Project
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef ADACHESS_NODES_HPP
#define ADACHESS_NODES_HPP

#include <cstdint>

namespace chess {

/**
 * Node type for counting nodes in search trees.
 * Uses a 64-bit unsigned integer (matches Ada's modular type 2^64).
 */
using Node = std::uint64_t;

/**
 * Maximum node count value.
 */
inline constexpr Node kMaxNodeCount = UINT64_MAX;

/**
 * Perft node type - for performance testing.
 * Handles up to perft depth 10 (max 2^46 nodes).
 *
 * This is needed for pretty print of perft and divide commands.
 * At 20 million NPS, it would take ~1000 hours to overflow.
 */
using PerftNode = std::uint64_t;

/**
 * Maximum perft node count.
 */
inline constexpr PerftNode kMaxPerftNodeCount = (1ULL << 46);

// ============================================================================
// Search Tree Node Types
// ============================================================================

/**
 * Tree node type classification for alpha-beta search.
 * Corresponds to Ada's Tree_Node_Type enumeration.
 *
 * - PvNode: Principal variation node (default)
 * - CutNode: Beta cut occurred (fail-high/all-node in minimax)
 * - AllNode: No cutoff occurred (fail-low/all-node in minimax)
 */
enum class TreeNode : std::uint8_t {
    PvNode  = 0,
    CutNode = 1,
    AllNode = 2
};

/**
 * Toggle the tree node type.
 * Corresponds to Ada's overloaded "not" operator.
 *
 * - PvNode remains PvNode
 * - CutNode becomes AllNode
 * - AllNode becomes CutNode
 *
 * @param node The tree node type to toggle.
 * @return The toggled node type.
 */
[[nodiscard]] constexpr TreeNode flip(TreeNode node) noexcept {
    switch (node) {
        case TreeNode::PvNode:  return TreeNode::PvNode;
        case TreeNode::CutNode: return TreeNode::AllNode;
        case TreeNode::AllNode: return TreeNode::CutNode;
    }
    return TreeNode::PvNode;  // Should never reach here
}

/**
 * Operator overload for tree node negation (alias for flip).
 *
 * @param node The tree node type to negate.
 * @return The toggled node type.
 */
[[nodiscard]] constexpr TreeNode operator~(TreeNode node) noexcept {
    return flip(node);
}

/**
 * Check if a tree node is a PV node.
 */
[[nodiscard]] constexpr bool is_pv_node(TreeNode node) noexcept {
    return node == TreeNode::PvNode;
}

/**
 * Check if a tree node is a cut node.
 */
[[nodiscard]] constexpr bool is_cut_node(TreeNode node) noexcept {
    return node == TreeNode::CutNode;
}

/**
 * Check if a tree node is an all node.
 */
[[nodiscard]] constexpr bool is_all_node(TreeNode node) noexcept {
    return node == TreeNode::AllNode;
}

} // namespace chess

#endif // ADACHESS_NODES_HPP
