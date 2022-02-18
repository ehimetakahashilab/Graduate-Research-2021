import sys
DICT_SIZE = 12

class TrieNode(object):
    """
    Trie木の1ノードを表すクラス
    """
    WORD_SIZE = 64 ## underbar + space + alphabets + number ##
    def __init__(self, depth = 0, item = None):
        self.item = item
        self.depth = depth
        # 子ノードのリストはとりあえずintリストとして保持 #
        self.children = [-1 for i in range(TrieNode.WORD_SIZE)]

    def is_leaf(self):
        """
        そのノードが葉であればTrueを,
        そうでなければFalseを返す
        """
        for index in self.children:
            if index != -1:
                return False
        return True


class Trie(object):
    """
    Trir木全体を表すクラス
    TrieNodeクラスと併用して用いる
        nodes自体は、あるノードと、その子ノードを持っている
        配列の添え字がアルファベットを表し、そこに入る番号がnode_indexを表す
    """
    def __init__(self, init_alphabets=None):
        ## 追加可能ノード数の計算 ##
        self.max_dict_cap = 2 ** DICT_SIZE
        #print(f"Maximum number of words : {self.max_dict_cap}")
        ## 根ノードの作成 ##
        root = TrieNode()
        self.nodes = [root]
        # rootのindexは0 #
        node_index = 0
        ## 初期辞書の登録 ##
        if init_alphabets == None:
            init_alphabets = list("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_ ")
        ## 出力文字列(int) ##
        #self.output_data = []
        for item_alp in init_alphabets:
            char_num = self._get_char_num(item_alp)
            new_node = TrieNode(depth = 1, item = item_alp)
            # ノード追加後なので、 1からWORD_SIZE-1 が帰ってくる #
            next_node_index = self._add_node(new_node)
            self.nodes[node_index].children[char_num] = next_node_index
        ## 初期辞書登録時の辞書bit_size(更新(未実装)を実装する場合に使用されたい) ##
        self.bit_size, self.max_dict_cap_n = self._near_pow2(len(init_alphabets))

    ## private ##
    def __ret_parent_node_index(self, node_index):
        """
        引数のindexのノードの、親ノードのイデックスを返す
        親ノードが存在しない場合はルートノードの親ノードを捜索したときのみであるため、
        エラーメッセージとともにプログラムは停止する
        """
        for i, t_node in enumerate(self.nodes):
            ## i: node_index,  t_node: target node ##
            for child_index in t_node.children:
                if child_index == node_index:
                    return i
        print("Unexpected error:親ノードが存在しないノードを参照:node_index=",end="")
        print(node_index)
        sys.exit(1)

    def _get_char_num(self, c):
        """
        文字のidを返す(配列実装のため)
        A:0, B:1, ..., Z:25, a:26, b:27, ..., z:51, 0:52, ..., 9:61, _:62, " ":63
        """
        ## 出現する特別な文字 ##
        if c == "_":
            return 62
        if c == " ":
            ## spaceのidは63 ##
            return 63
        ## アルファベット ##
        if c.isalpha():
            if c.isupper():
                return ord(c) - ord('A')
            if c.islower():
                return ord(c) - ord('a') + 26
        ## 数字も出現したため追加実装 ##
        if c.isdecimal():
            return ord(c) - ord('0') + 52
        ## いずれかに分類されない場合は読み込みエラー ##
        print(f"読み込めない文字:{c}")
        sys.exit(1)

    def _add_node(self, node):
        """
        nodesのノードを追加する
        """
        self.nodes.append(node)
        return len(self.nodes) - 1


    def _delete_node(self, index):
        """
        指定indexのノードを削除する
        1. ノード自身の削除
        2. 親ノードの子ノードリストからの削除
        3. 子ノードリストの番号の再割り当て(なくなった番号以降のノードインデックスを1減算する)
        """
        ## ノード自身の削除 ##
        del self.nodes[index]
        ## 親ノードの子ノードリストから削除 ##
        error_flg = True ## 子ノードの削除検知 ##
        parent_node_index = self.__ret_parent_node_index(index)
        for i, child_index in enumerate(self.nodes[parent_node_index].children):
            if child_index == index:
                ## initialize ##
                self.nodes[parent_node_index].children[i] = -1
                error_flg = False
                break
        ## 子ノードリストから何も削除されない場合は異常 ##
        if error_flg:
            print("Unexpected error:親ノードの保有する子ノードリストからの削除に失敗")
            sys.exit(1)
        ## 子ノードリストの番号再割り当て ##
        for i, t_node in enumerate(self.nodes):
            for j, child_index in enumerate(t_node.children):
                if child_index == index:
                    print("Unexpected error:親ノードの保有する子ノードリストからの削除が不完全")
                    self.output_tree(0)
                    sys.exit(1)
                if child_index > index:
                    self.nodes[i].children[j] -= 1

    def _near_pow2(self, n):
        """
        ビット数更新プログラム
        """
        if n <= 0:
            return 0
        bit_size = 1
        i = 2
        while i <= n:
            bit_size += 1
            i *= 2
        return bit_size, i

    ## methods ##
    def encoding(self, word, char_index = 0, node_index = 0, stdout=False):
        """
        Trieに新しい単語を"登録せず"に符号後を返す
        Parameterはinsert_encodingと同じ
        """
        output_data = []
        while True:
            parent_depth = self.nodes[node_index].depth
            char_num = self._get_char_num(word[char_index])
            next_node_index = self.nodes[node_index].children[char_num]
            if next_node_index == -1: ## 辿れなくなった場合 ##
                ## 符号語の出力 & 出力の保存 ##
                if stdout:
                    print("{0}".format(node_index), end=" ")
                output_data.append(node_index)
                node_index = 0
                continue
            else: ## 辿れる時 ##
                if char_index < len(word) - 1:
                    char_index += 1
                    node_index = next_node_index
                    continue
                else: ## 最後の文字であれば ##
                    if stdout:
                        print(next_node_index)
                    output_data.append(next_node_index)
                    break
        return output_data

    def output_tree(self, node_index, depth = 0):
        """
        Trie木の表示
        debug 及び 動作確認 用
        """
        ##############
        def __my_space(depth):
            for i in range(depth):
                if i != 0:
                    print("  ", end="")
        ##############

        child_list = []
        if node_index == 0:
            print("\n親ノード：root\n")
        else:
            print()
            __my_space(depth)
            print("Node: {0}".format(node_index))
        __my_space(depth)
        print("  depth: {0}".format(self.nodes[node_index].depth))
        __my_space(depth)
        print("  item: {0}".format(self.nodes[node_index].item))
        for itr, x in enumerate(self.nodes[node_index].children):
            if x != -1: ## 次の頂点が存在すれば
                __my_space(depth)
                print("  child node: {0}".format(x))
                child_list.append(x)
        for x in child_list:
            self.output_tree(x, depth+1)


### LZW関連 ###
class Trie_LZW(Trie):
    """
    LZWでの圧縮
    """
    def __init__(self, init_alphabets=None):
        super().__init__(init_alphabets)

    def insert_encoding(self, word, char_index=0, node_index=0, stdout=False):
        """
        Trieに新しい単語を登録して、符号語を返す
        word : 登録する文字列
        item : 保持するアルファベット
        char_index : 現在着目している文字の位置
        stdout : 標準出力するかどうかを選択
        bit数で辞書登録を停止する処理を追加
        """
        output_data = []
        while True:
            parent_depth = self.nodes[node_index].depth
            char_num = super()._get_char_num(word[char_index])
            next_node_index = self.nodes[node_index].children[char_num]
            if next_node_index == -1: ## 辿れなくなった場合 ##
                ## 符号語の出力 ##
                if stdout:
                    print("{0}".format(node_index), end=" ")
                output_data.append(node_index)
                if len(self.nodes) - 1 < self.max_dict_cap:  ## 辞書が満杯でないとき ##
                    ## 辿れなくなった語を木に追加 ##
                    new_node = TrieNode(parent_depth+1, item = word[char_index])
                    next_node_index = super()._add_node(new_node)
                    self.nodes[node_index].children[char_num] = next_node_index
                else:
                    pass
                node_index = 0
                continue
            else:
                if char_index < len(word) - 1:
                    char_index += 1
                    node_index = next_node_index
                    continue
                else:
                    if stdout:
                        print(next_node_index)
                    output_data.append(next_node_index)
                    break
        return output_data


### LZT関連 ###
class Trie_LZT(Trie):
    """
    こんな感じでLZTを追加してもらって
    """
    def __init__(self, init_alphabets=None):
        ## 初期化とか ##
        pass

    def insert_encoding(self, word, char_index=0, node_index=0, stdout=False):
        ## 符号化処理とか ##
        pass


if __name__ == '__main__':
    trie = Trie_LZW()
    apicalls = "aaaaa ababa 12abc ababa abcde"
    out = trie.insert_encoding(apicalls, stdout=True)
    outsize = 12 * len(out)
    print(f"size:{outsize}")