#ifndef __CUSTOMBINARYTREE_HPP__
#define __CUSTOMBINARYTREE_HPP__

#include <memory>
#include <queue>
#include <vector>
#include <functional>
#include <iostream>

/**
 * @file customBinaryTree.hpp
 * @brief 二叉树的一些理解和实现
 *
 * perfect binary tree: 所有层的节点都是满的
 * complete binary tree: 除最后一层外，每一层都是满的, 且最后一层的节点从左往右依次排列
 * full binary tree: 每个节点的度不是0就是2
 * balanced binary tree: 每个节点的左右子树高度差不超过1
 * binary search tree: 对于任意节点, 左子树的节点值都小于该节点值, 右子树的节点值都大于该节点值
 * AVL binary tree: 自平衡二叉搜索树, 任意节点的左右子树高度差不超过1(即平衡因子不超过1)
 */

namespace tree
{
    enum class BinaryTreeType
    {
        BinaryTree,
        BalancedBinaryTree,
        BinarySearchTree,
        AVL,
    };

    template<typename T>
    class IBinaryTree;

    template<typename T>
    class BinaryTree;

    template<typename T, BinaryTreeType btype = BinaryTreeType::BinaryTree>
    class customBinaryTree
    {
    public:
        customBinaryTree()
        {
            if constexpr(btype == BinaryTreeType::BinaryTree)
                m_binary_tree = std::make_unique<BinaryTree<T>>();
        }
        void insert(const T& v)
        {
            if (!m_binary_tree)
                return;

            m_binary_tree->insert(v);
        };

        void remove(const T& v)
        {
            if (!m_binary_tree)
                return;

            m_binary_tree->remove(v);
        }

        std::size_t find(const T& v) const 
        {
            if (!m_binary_tree)
                return 0;

            return m_binary_tree->find(v);
            
        };

        void clear() 
        {
            if (!m_binary_tree)
                return;

            m_binary_tree->clear();
        };

        virtual std::size_t size() const
        {
            if (!m_binary_tree)
                return 0;

            return m_binary_tree->size();
        }

        void print() const
        {
            if (!m_binary_tree)
                return;

            return m_binary_tree->print();
        }

        constexpr BinaryTreeType type() const {return btype;}
    private:
        std::unique_ptr<IBinaryTree<T>> m_binary_tree{nullptr};
    };

    template<typename T>
    class IBinaryTree
    {
    public:
        struct Node
        {
            T value;
            std::shared_ptr<Node> left;
            std::shared_ptr<Node> right;

            Node(const T& val, 
                std::shared_ptr<Node> _left = nullptr, 
                std::shared_ptr<Node> _right = nullptr) : value(val), left(_left), right(_right) {}
        };
    public:
        virtual ~IBinaryTree() = default;
        virtual void insert(const T& value) = 0;
        virtual void remove(const T& value) = 0;
        virtual std::size_t find(const T& value) = 0;
        virtual void clear() = 0;
        virtual std::size_t size() const = 0;
        virtual void print() const = 0;

        /**
         * @brief 获取最远叶子结点,相同距离选右节点
         * @param[in] node 起始节点
         * @return 最远叶子结点
         */
        static std::shared_ptr<Node> getFurthestLeafNode(std::shared_ptr<Node> node)
        {
            if (!node)
                return nullptr;

            std::vector<std::shared_ptr<Node>> longest_node2leaf_path;
            std::function<std::size_t(std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>&)> findLongestPathToLeaf = [&](std::shared_ptr<Node> node, std::vector<std::shared_ptr<Node>>& nodes)->std::size_t
            {
                if (!node)
                    return 0;

                std::vector<std::shared_ptr<Node>> left_nodes;
                std::vector<std::shared_ptr<Node>> right_nodes;
                auto left_nums = findLongestPathToLeaf(node->left, left_nodes);
                auto right_nums = findLongestPathToLeaf(node->right, right_nodes);

                nodes.push_back(node);
                if (left_nums > right_nums)
                {
                    nodes.insert(std::end(nodes), std::begin(left_nodes), std::end(left_nodes));
                    return left_nums + 1;
                }
                else
                {
                    nodes.insert(std::end(nodes), std::begin(right_nodes), std::end(right_nodes));
                    return right_nums + 1;
                }
            };

            auto nums = findLongestPathToLeaf(node, longest_node2leaf_path);

            return nums ? longest_node2leaf_path.back() : nullptr;
        }

        /**
         * @brief 层序遍历
         * @param[in] node 任意节点
         * @param[in] callback 回调函数
         */
         static void BFS(std::shared_ptr<Node> node, std::function<bool(const T&)> callback = [](const T& value){std::cout<<value<<" "; return false;})
         {
             if (!node)
                 return;

             std::queue<std::shared_ptr<Node>> queue;
             queue.push(node);

             while(!queue.empty())
             {
                 auto node = queue.front();
                 queue.pop();

                 if (callback(node->value))
                    break;

                 if (node->left)
                     queue.push(node->left);
                 if (node->right)
                     queue.push(node->right);
             }
         }
        
        enum class DFS_Type{PreOrder, InOrder, PostOrder};
        /**
         * @brief Depth-First Search(DFS), 深度优先搜索
         * @param[in] node 任意节点
         * @param[in] type 对应前序,中序,后序遍历
         */
        static void DFS(std::shared_ptr<Node>& node, DFS_Type type = DFS_Type::PreOrder,
        std::function<void(std::shared_ptr<Node>& node)> callback = [](std::shared_ptr<Node>& node){std::cout<<node->value<<" ";})
        {
            switch(type)
            {
                case DFS_Type::PreOrder:
                {
                    std::function<void(std::shared_ptr<Node>&)> preorder = [&](std::shared_ptr<Node>& node)
                    {
                        if (!node)
                            return;

                        callback(node);
                        preorder(node->left);
                        preorder(node->right);
                    };
                    preorder(node);
                }
                break;
                case DFS_Type::InOrder:
                {
                    std::function<void(std::shared_ptr<Node>&)> inorder = [&](std::shared_ptr<Node>& node)
                    {
                        if (!node)
                            return;

                        inorder(node->left);
                        callback(node);
                        inorder(node->right);
                    };
                    inorder(node);
                    break;
                }
                case DFS_Type::PostOrder:
                {
                    std::function<void(std::shared_ptr<Node>&)> postorder = [&](std::shared_ptr<Node>& node)
                    {
                        if (!node)
                            return;

                        postorder(node->left);
                        postorder(node->right);
                        callback(node);
                    };
                    postorder(node);
                    break;
                }
                default:
                    break;
                
            }
        }
    };

    template<typename T>
    class BinaryTree: public IBinaryTree<T>
    {
    public:
        using Node = typename IBinaryTree<T>::Node;
        using Base = IBinaryTree<T>;
    public:
        virtual ~BinaryTree() = default;

        /**
         * @brief 插入节点,按照层序遍历有空位置就插入
         * @param[in] value 待插入的节点值
         */
        virtual void insert(const T& value) override
        {
            if (!m_root)
                m_root = std::make_shared<Node>(value);
            else
            {
                std::queue<std::shared_ptr<Node>> queue;
                queue.push(m_root);

                while(!queue.empty())
                {
                    auto node = queue.front();
                    queue.pop();

                    if (!node->left)
                    {
                        node->left = std::make_shared<Node>(value, nullptr, nullptr);
                        break;
                    }
                    else if(!node->right)
                    {
                        node->right = std::make_shared<Node>(value, nullptr, nullptr);
                        break;
                    }
                    else
                    {
                        queue.push(node->left);
                        queue.push(node->right);
                    }
                }
            }
            ++m_size;
        }

        /**
         * @brief 删除匹配value的节点
         *
         *  删除节点需要考虑的情况:
         *  1. 节点是叶子节点则直接删除,更新叶子结点的父节点的指向nullptr
         *  2. 节点是只有一个子节点则子节点值替换当前节点值,并更新当前节点的子节点指向nullptr
         *  3. 节点有两个子节点则找到最远叶子节点,替换当前节点值,并删除最远叶子节点, 更新叶子结点的父节点指向nullptr
         *
         * @note 使用后序遍历方便删除节点因为可以方便更新根节点的左右指向, 也可以使用层序遍历不过会增加空间复杂度
         *
         * @param[in] value 待删除的节点值
         */
        virtual void remove(const T& value) override
        {
            if (!m_root)
                return;

            std::function<std::shared_ptr<Node>(std::shared_ptr<Node>, const T&)> delete_node = [&](std::shared_ptr<Node> node, const T& value)->std::shared_ptr<Node>
            {
                if (!node)
                    return nullptr;

                node->left = delete_node(node->left, value);
                node->right = delete_node(node->right, value);

                if (node->value == value)
                {
                    if (!node->left && !node->right)
                        return --m_size, nullptr;
                    else if (node->left && !node->right)
                        return --m_size, node->left;
                    else if (!node->left && node->right)
                        return --m_size, node->right;
                    else
                    {
                        auto furthest_leaf_node = IBinaryTree<T>::getFurthestLeafNode(node);
                        node->value = furthest_leaf_node->value;
                        delete_node(node, furthest_leaf_node->value);
                    }
                }

                return node;
            };

            delete_node(m_root, value);
        }

        /**
         * @brief 查找二叉树内匹配value的节点数
         * @param[in] value 查找的值
         * @return 返回匹配的节点数
         */
        virtual std::size_t find(const T& value) override
        {
            std::size_t count{0};
            Base::DFS(m_root, Base::DFS_Type::PreOrder, [&](auto& node){if (node->value == value) ++count;});
            return count;
        }

        /**
         * @brief 通过后序遍历释放节点元素
         */
        virtual void clear() override
        {
            if (m_root)
                m_root.reset();
        }

        virtual std::size_t size() const override {return m_size;}

        virtual void print() const override {Base::BFS(m_root);}
    private:
        std::size_t m_size{0};
        std::shared_ptr<Node> m_root{nullptr};
    };


}

#endif // __CUSTOMBINARYTREE_HPP__