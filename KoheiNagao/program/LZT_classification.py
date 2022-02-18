import os, sys, json, shutil, random, copy, pickle, numpy
import pandas as pd
from tqdm import tqdm
from collections import defaultdict
from sklearn import svm, preprocessing, metrics
from sklearn.metrics import confusion_matrix, precision_score, recall_score
from sklearn.model_selection import GridSearchCV
from sklearn.ensemble import RandomForestClassifier
from halo import Halo

DICT_SIZE = 12
HEAD = 0



"""
~要素個数問題について~
辞書に保存できるノードの個数は 2^const.DICT_SIZE 個 (例えば2^2 = 4個)
TrieNodeのcapは2^const.DICT_SIZE(例:4)であり、通常であれば0~3の4個のデータを保存できることになる。
しかし、0はrootが使用しているため、1~3の3個しかノードは保存することができない。
したがって、len(nodes) - 1 によってrootの分を引いた時のノード数が、2^const.DICT_SIZE(例:4)未満となるようにすることで、個数制限を実装
4のときは、更に追加すると溢れるため、4未満
"""

"""
汎用関数
"""
##############################
def ReadApiCall(filename):
	with open(filename, "r") as f:
		data = f.read()
	return data

def RandomNoDuplicate(min, max, num, remove_list=list()):
	"""
	min以上、max以下の数字を重複を許容せずランダムにnum個選択する
	min : 返す値の最小値
	max : 返す値の最大値
	num : 返す値の個数
	remove_list : min以上max以下の値の中で排除する数字(int型)
	nd_list : 重複を許容しない数字リスト
	"""
	## 引数の検査 ##
	if ((max - min + 1) - len(remove_list)) < num:
		print("Error:最大値・最小値・削除リストの値が不正")
		print(f"{max} {min} {num} {remove_list} {len(remove_list)}")
		sys.exit(1)
	## num個のランダムな値を取得 ##
	nd_list = []
	while len(nd_list) < num:
		x = random.randint(min, max)
		if (x not in nd_list) and (x not in remove_list):
			nd_list.append(x)
	return nd_list

def CompressionRatio(output_data, apicalls, gamma, dict_size):
	"""
	int型の出力数値列を受け取り、DICT_SIZEとの兼ね合いで
	圧縮後のサイズを出力する
	"""
	ASCII_SIZE = 8 ## 1文字8bit ##

	## 圧縮前のデータサイズ ##
	api_size = len(apicalls) * ASCII_SIZE

	## 圧縮後のデータサイズ ##
	data_size = len(output_data) * dict_size

	## 対象ファミリ圧縮率 ##
	compress_ratio = (data_size + gamma) / (api_size + gamma)

	return compress_ratio

def OutCsvFile(df, path="./", file_name="NewFile"):
	"""
	データフレームのCSVファイル出力関数
	df: 対象データフレーム (pandas.df)
	buffer_path: 出力ファイルパス
	buffer_file_name="出力ファイル名"
	"""
	print("Save the csv file about compression ratio ...")
	if path[-1] != "/":
		path = path + "/"
	## 結果の出力 ##
	file = path + file_name + ".csv"
	print(f"Save to : {file}")
	df.to_csv(file)
	print("Collect!", end="\n\n")

def ReadCsvFile(filepath):
	"""
	csvファイルをpd_dataframeにして返す
	0行目にヘッダ、0列目にindexがあることを想定
	"""
	df = pd.read_csv(filepath, header=0, index_col=0)
	return df

def GetFamilyList(dataset_path):
	return [family_name for family_name in os.listdir(dataset_path)]
##############################






################################################################### LZT関連 ################################################################################
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
            print("\n親ノード:root\n")
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

## 双方向リストによるキュー ##
class Queue:

    def __init__(self):
        self.prev = [None] * (2 ** DICT_SIZE + 1)
        self.next = [None] * (2 ** DICT_SIZE + 1)
        self.prev[HEAD] = HEAD
        self.next[HEAD] = HEAD
        self.data_num = 0

    ## 最後尾に追加 ##
    def insert(self, x):
        last = self.prev[HEAD]
        self.prev[x] = last
        self.next[x] = HEAD
        self.next[last] = x
        self.prev[HEAD] = x
        self.data_num += 1

    ## 削除 ##
    def delete(self, x):
        p = self.prev[x]
        q = self.next[x]
        self.next[p] = q
        self.prev[q] = p

    ## 巡回 ##
    def traverse(self):
        n = self.next[HEAD]
        while n != HEAD:
            yield n
            n = self.next[n]

    def slide_number(self, n):
        for i, item in enumerate(self.prev):
            if item == None:
                continue
            if item > n:
                self.prev[i] -= 1
            if i > n:
                self.prev[i-1] = self.prev[i]

        for i, item in enumerate(self.next):
            if item == None:
                continue
            if item > n:
                self.next[i] -= 1
            if i > n:
                self.next[i-1] = self.next[i]




### LZT関連 ###
class Trie_LZT(Trie):
    """
    こんな感じでLZTを追加してもらって
    """
    def __init__(self, init_alphabets=None):
        super().__init__(init_alphabets)
        self.queue = Queue()
        for index in range(1, len(self.nodes)):
            self.queue.insert(index)

    def insert_encoding(self, word, char_index=0, node_index=0, stdout=False):
        output_data = []
        while True:
            ## 子ノードの深さを与えるために、親の深さを取得 ##
            parent_depth = self.nodes[node_index].depth

            ## 文字の番号表現を取得 ##
            char_num = super()._get_char_num(word[char_index])

            ## 現在のノードの子ノードに、次の文字が存在するかを確認 ##
            next_node_index = self.nodes[node_index].children[char_num]

            ## 辿れなくなった場合 ##
            if next_node_index == -1:
                if stdout:
                    print("{0}".format(node_index), end = " ")
                output_data.append(node_index)

                ## 辞書が埋まった場合 ##
                if len(self.nodes)-1 == self.max_dict_cap:
                    for s_node_index in self.queue.traverse():
                        
                        ## 葉であるかの確認 ##
                        if self.nodes[s_node_index].is_leaf() and self.nodes[s_node_index].depth != 1:
                            
                            ## 使用されていない辞書情報の削除 ##
                            super()._delete_node(s_node_index)
                            self.queue.delete(s_node_index)
                            self.queue.slide_number(s_node_index)
                            if node_index > s_node_index:
                                node_index -= 1
                            break
                
                ## 辿れなくなった語を木に追加 ##
                new_node = TrieNode(parent_depth+1, item = word[char_index])
                next_node_index = super()._add_node(new_node)
                self.nodes[node_index].children[char_num] = next_node_index

                ## 登録した辞書番号をqueueに追加 ##
                self.queue.insert(next_node_index)
                node_index = 0
                continue


            ## 辿れる場合 ##
            else:
                if char_index < len(word) - 1:
                    self.queue.delete(next_node_index)
                    self.queue.insert(next_node_index)
                    char_index += 1
                    node_index = next_node_index
                    continue

                ## 最後の文字 ##
                else:
                    if stdout:
                        print(next_node_index)
                    output_data.append(next_node_index)
                    break
        return output_data


"""
実験用関数
- データセット作成関連:PreprocessDataset
- 辞書登録:RegistrationFeatureExtraction
- 圧縮率の計算:CalculateCompressRatio
~参考~
pickle : https://www.delftstack.com/ja/howto/python/python-save-dictionary/
"""
##############################
## データセット作成関連 ##
def PreprocessDataset(dataset_path="./Dataset/", buffer_path="./Buffer/", read_mode=False, LZW_flag=False):
	"""
	データセットの検体を，特徴抽出用検体と実験用検体に分ける
	返り値:sample_feature_ex(特徴抽出用検体）, dataset_list_as（実験用検体[ファミリ毎に1:1に成形済み]）
	連想配列によって束ねたものを返すが、その形式は実験モードにより異なるため，形式を変更した場合は本関数以下の関数も修正が必要である(改正済み)
	dataset_path : データセットのフォルダパス
	buffer_path : 出力バッファのフォルダパス
	read_mode : 既にデータセットを作成している上で、その条件を変更せずpickleを使用してバッファから読み込み実験を行う場合はTrue
	"""
	MINIMUM_SAMPLE_NUM = 100 ## データセットの最小許容検体数
	FEATURE_EXTRACTION_MULTIV = 10 ## 特徴抽出用検体数(LZT)

	## inner function ### 
	def MakeDatasetInformation(dataset_path):
		family_counter_as = {}
		for family_name in os.listdir(dataset_path):
			## ファイルの個数を調査 ##
			cur_dir = dataset_path+family_name+"/"
			sample_num = sum(os.path.isfile(os.path.join(cur_dir, file)) for file in os.listdir(cur_dir))
			if sample_num < MINIMUM_SAMPLE_NUM:
				print(f"Unexpected error: 検体数が不足 (MINIMUM_SAMPLE_NUM:{MINIMUM_SAMPLE_NUM}")
				sys.exit(1)
			family_counter_as[family_name] = sample_num
		return family_counter_as

	def GetShortApiSamples(dataset_path, minimum):
		short_sample_as = {}
		for family_name in os.listdir(dataset_path):
			short_sample_as[family_name] = []
			cur_dir = dataset_path+family_name+"/"
			for file in os.listdir(cur_dir):
				## ファイルサイズを取得 ##
				size = os.path.getsize(cur_dir+file)
				if size < minimum:
					## 検体番号取得関数がint型で受け取るため，整数値で格納 ##
					short_sample_as[family_name].append(int(file.split(".")[0]))
		return short_sample_as

	def OutBufferFile(as_array, out_path=None, filename="Buffer", out_mode="pkl"):
		"""
		同一検体での実験再現を可能とするため、外部ファイルへ保存する
		ここでは漬物化、可視性を考慮してjsonでも良い
		"""
		## モードチェック ##
		if out_path != None:
			if out_path[-1] != "/":
				out_path = out_path + "/"
			if not os.path.exists(out_path):
				print(f"Error : フォルダが存在しません({dataset_path})")
				sys.exit(1)
			if not os.path.isdir(out_path):
				print(f"Error : フォルダではありません({out_path})")
				sys.exit(1)
		else:
			out_path = "./"
		if out_mode == "pkl":
			## pickle で保存 ##
			with open(out_path+filename+".pkl", "wb") as f:
				pickle.dump(as_array, f)
		elif out_mode == "json":
			with open(out_path+filename+".json", "w") as f:
				json.dump(as_array, f, indent=4)
	#####################

	print("Preparing the dataset ...")

	## 読み込みモード ##
	### jsonに修正　##
	if read_mode:
		## とりあえず必要ないので省略 ##
		print("Error : Skip implementation.")
		sys.exit(1)

	## 作成モード(デフォルト) ##
	## フォルダの準備 ##
	if dataset_path[-1] != "/":
		dataset_path += "/"
	if not os.path.exists(dataset_path):
		print(os.listdir("./"))
		print(f"Error : フォルダが存在しません({dataset_path})")
		sys.exit(1)
	if os.path.exists(buffer_path):
		print(f"フォルダの中身が削除されます:{buffer_path}")
		print("(y/n):", end="")
		y_n = input().strip()
		if y_n == "y" or y_n == "yes":
			shutil.rmtree(buffer_path)
		else:
			print("Process Stop...")
			sys.exit(1)
	os.makedirs(buffer_path)

	## 各ファミリの検体数を取得 (連想配列{str(familyname):int(num)}) ##
	family_counter_as = MakeDatasetInformation(dataset_path)
	sample_feature_ex = {}
	dataset_list_as = {}

	## 検体の比率調整パラメータ(ファミリ数が32だったら、31を作っておけば31:31が表現できるようになる) ##
	append_parameter = len(family_counter_as) - 1
	## 整合性検査(少なくとも特徴抽出(append_parameter) + 1データセット単位(appendparameter)は必要) ##
	if 2*append_parameter > MINIMUM_SAMPLE_NUM:
		print("検体数が不足")
		sys.exit(1)
	t_remove_list_as = {} ## 重複防止用検体番号格納変数
	## sample_feature_ex:{str(target family):list[sample file]}
	## LZWの場合の12ビット分保存できるまで検体を取得するモード ##
	if LZW_flag == True:
		for t_family, sample_num in family_counter_as.items():
			## 長尾君のでは必要ないので省略 ##
			print("Error : Skip implementation.")
			sys.exit(1)
	## 10検体を見るモード ##
	else:
		for t_family, sample_num in family_counter_as.items():
			x1 = RandomNoDuplicate(1, sample_num, FEATURE_EXTRACTION_MULTIV)
			t_remove_list_as[t_family] = list(x1)
			sample_feature_ex[t_family] = [dataset_path+t_family+"/"+str(i)+".txt" for i in x1]
			max_sample_num = sample_num - FEATURE_EXTRACTION_MULTIV

	## この辺で32:32のデータセットを作成してる筈 ##
	remove_list_extend = {}
	for t_family in family_counter_as.keys():
		dataset_list_as[t_family] = {}
		remove_list_extend[t_family] = copy.deepcopy(t_remove_list_as)
		print(f"  target:{t_family}")
		gap_flg = True ## 次回実行可能判定フラグ ##
		while gap_flg:
			for key, sample_num in family_counter_as.items():
				if key not in dataset_list_as[t_family].keys():
					dataset_list_as[t_family][key] = []
				## 対象ファミリの場合 ##
				if key == t_family:
					## 特徴抽出用検体の検体数 ##
					feature_extraction_num = len(sample_feature_ex[key])
					## append_parameter個のkey内乱数取得(特徴抽出用検体を除く) ##
					x = RandomNoDuplicate(1, sample_num, append_parameter, remove_list=remove_list_extend[t_family][key])
					filename = [dataset_path+key+"/"+str(i)+".txt" for i in x]
					dataset_list_as[t_family][key].extend(filename)
					next_len = len(dataset_list_as[t_family][key]) + append_parameter
				## 対象ファミリ以外の場合 ##
				else:
					## 特徴抽出用検体の検体数 ##
					feature_extraction_num = len(sample_feature_ex[key])
					x = RandomNoDuplicate(1, sample_num, 1, remove_list=remove_list_extend[t_family][key])
					filename = [dataset_path+key+"/"+str(i)+".txt" for i in x]
					dataset_list_as[t_family][key].extend(filename)
					next_len = len(dataset_list_as[t_family][key]) + 1
				## 重複防止連想配列の更新 ##
				remove_list_extend[t_family][key].extend(x)
				## 次回実行可能判定 ##
				if next_len > sample_num - feature_extraction_num:
					gap_flg = False

	## 選択した検体の出力 ##
	OutBufferFile(sample_feature_ex, out_path=buffer_path, filename="FeatureExtractionLists_MultiClassification", out_mode="json")
	OutBufferFile(dataset_list_as, out_path=buffer_path, filename="DatasetLists_MultiClassification", out_mode="json")

	print("Collect!", end="\n\n")
	return sample_feature_ex, dataset_list_as


## 辞書登録関連 ##		
def RegistrationFeatureExtractionMultivalued(sample_as, compress_mode="lzt"):
	"""
	選択された特徴抽出用検体を引数にとり(sample_as)、辞書に登録して、ファミリ毎の辞書の連想配列を返す。
	"""
	print("Registering to the Trie tree ...")
	trie_as = {} ## dict{str(family):Trie obj}
	for t_family, t_sample_list in tqdm(sample_as.items()):
		## targetファミリ用のTrie tree ##
		if compress_mode == "lzt":
			target_trie = Trie_LZT()
		else:
			print("Error : Skip implementation.")
			sys.exit(1)
		for t_sample in t_sample_list:
			apicalls = ReadApiCall(t_sample).strip()
			target_trie.insert_encoding(apicalls)
		trie_as[t_family] = target_trie
	return_value = trie_as
	print("Collect!!", end="\n\n")
	return return_value ## dict{trie trees of feature extraction}


## 圧縮率計算関連 ##
def CalculateCompressRatioMultivalued(trie_as, dataset_as, gamma=0, dict_size_check=False, stdout=False, buffer_path="./Buffer/"):
	"""
	CaluculateCompressRatioBinaryの多次元ベクトルversion
	後でdataframeを簡単に扱うために、同じ検体であっても再計算する方式をとってる。
	実行時間が果てしなく長くなるので、先に圧縮率を計算してからdataframe化した方がよい。
	その実装については、時間に余裕がないので、また検討されたい。
	"""
	print("Calculation of compression ratio ...")

	## 出力結果データフレームの準備 ##
	df_result = pd.DataFrame({
	"target_family":[],
	"sample_family":[],
	"label":[],
	"sample_file_path":[],
	})
	for item in trie_as.keys():
		df_result["Z_"+str(item)] = []

	## 圧縮率を求め、データフレームに出力 ##
	## trie_as : {"family":obj(trietree), "family":obj...} ##
	## dataset_as : {targetfamily:{family:[datafile path]}} ##
	counter = 1
	for t_family, dataset in dataset_as.items():
		print(f"target family : {t_family} ... ({counter}/{len(dataset_as)})")
		counter+=1
		for family, path_list in tqdm(dataset.items()):
			if dict_size_check:
				print("未実装")
				sys.exit(1)
			else:
				pass
			for data_path in path_list:
				## apiコールの取得 ##
				apicalls = ReadApiCall(data_path).strip()
				## treeに対してencoding ##
				outputs = {}
				z_family = []
				for fam in trie_as.keys():
					outputs[fam] = trie_as[fam].encoding(apicalls)
					## 圧縮率の計算 ##
					z_family.append(CompressionRatio(outputs[fam], apicalls, gamma, DICT_SIZE))
				if stdout:
					print("未実装")
					sys.exit(1)
				## ラベルの付与 ##
				if t_family == family:
					label = "target"
				else:
					label = "other"
				## 出力用DataFrameへの追加 ##
				buf_list = [t_family, family, label, data_path]
				buf_list.extend(z_family)
				df_result.loc[len(df_result)] = buf_list
			pass
		print("")
		pass

	## 結果のcsv出力 ##
	OutCsvFile(df_result, path=buffer_path, file_name="CompressionRatio")

	print("Collect!", end="\n\n")
	return df_result


## 散布図出力関連 ##
# 出力フォルダ準備 #
def PrepareOutputFolder(result_path):
	print("Error : Skip implementation.")
	sys.exit(1)
##############################


"""
機械学習関連
"""
##############################
class ANT_MachineLearning:
	def __init__(self, buffer_path, result_path, k=4, repro=False, scaling=True, stdout=True):
		"""
		SVM学習・検証用クラス
		buffer_path : bufferファイル出力先フォルダ
		result_path : 結果出力フォルダ
		k : k-fold validation の k
		exoeriment_mode : 実験モード(binary or multivalued)
		"""
		self.buffer_path = buffer_path
		self.k = k

		## 保存フォルダの形式統一 ##
		if result_path[-1] != "/":
			result_path = result_path + "/"
		## SVM用のフォルダ階層作成 ##
		self.result_path = result_path
		## Spreadsheet用のフォルダ階層作成 ##
		self.spread_path = result_path + "SpreadSheet/"

		## buffer_pathの形式統一 ##
		if buffer_path[-1] != "/":
			buffer_path = buffer_path + "/"
		self.buffer_path = buffer_path

		"""
		[Warning!]
		Change the "gridparameter" carefully, 
		because it may change the value of the replicated experiment later.
		"""
		#self.grid_parameters = [
		#{'C': [1, 5, 10, 50],
		#'gamma': [0.001, 0.0001]}
		#]
		self.grid_parameters = [
		{'C':[2**i for i in range(-5, 16)],
		'gamma':[2**i for i in range(-15, 4)]}
		]
		self.scaling = scaling
		self.stdout = stdout

		self.accuracy_avg = -1.0
		print("")


	## main methods ##
	def MachineLearningSvmExtend(self, df): ## LZWで平均正解率 87% 程度 ##
		## 32次元での2値分類の実験を行うメソッド ##
		if self.stdout:
			print("Classification by svm (binary extend)...")
		result_path = self.result_path + "SVM/"
		spread_path = self.spread_path + "SVM/"
		if os.path.exists(result_path):
			if self.stdout:
				print(f"Delete folder : {result_path}")
			shutil.rmtree(result_path)
		os.makedirs(result_path)
		buffer_path = self.buffer_path + "SVM/"
		if os.path.exists(buffer_path):
			if self.stdout:
				print(f"Delete folder : {buffer_path}")
			shutil.rmtree(buffer_path)
		os.makedirs(buffer_path)
		if os.path.exists(spread_path):
			if self.stdout:
				print(f"Delete folder : {spread_path}")
			shutil.rmtree(spread_path)
		os.makedirs(spread_path)

		if self.stdout:
			print(f"The result file will be saved : {result_path}")
			print(f"The buffer file will be saved : {buffer_path}", end="\n\n")
		## Check工程は省いたので自分で確認しましょう ##

		## 結果保存用 ##
		accuracy_dict = {}
		recall_dict = {}
		precision_dict = {}

		## 対象ファミリ毎の実験 ##
		family_length = len(df.groupby('target_family').groups)
		for ite, family in enumerate(df.groupby('target_family').groups):
			if self.stdout:
				print(f"target family : {family} ({ite+1}/{family_length})")
			## 再現用バッファ ##
			buffer_dict = {}
			b_json_file_name = buffer_path + family + ".json"
			b_json_f = open(b_json_file_name, "w")
			## 結果保存用 ##
			result_dict = {}
			json_file_name = result_path + family + ".json"
			json_f = open(json_file_name, "w")
			accuracy_dict[family] = []
			recall_dict[family] = []
			precision_dict[family] = []

			## 分割交差検証用にk個に分ける ##
			x_all_dict, t_all_dict, b_all_dict = self.__PreprocessForClf(df.groupby('target_family').get_group(family))

			## k-fold cross validation ##
			for k_count in range(self.k):
				if self.stdout:
					print(f"k-fold cross validation : ({k_count+1}/{self.k})...")
				buffer_dict['CV_'+str(k_count+1)] = {}
				result_dict['CV_'+str(k_count+1)] = {}
				## train test split ##
				x_train, t_train, b_train = [], [], []
				for key in b_all_dict.keys():
					if key == k_count:
						x_test, t_test, b_test = x_all_dict[key], t_all_dict[key], b_all_dict[key]
					else:
						x_train.extend(x_all_dict[key])
						t_train.extend(t_all_dict[key])
						b_train.extend(b_all_dict[key])
				## 学習データの依存関係保持シャッフル ##
				x_train, t_train, b_train = self.__ShuffleKeepDepend(x_train, t_train, b_train)
				## ndarray化 ##
				x_train, t_train = self.__ToNumpyForClf(x_train, t_train)
				x_test, t_test = self.__ToNumpyForClf(x_test, t_test)
				## スケーリング ##
				if self.scaling:
					x_train, x_test = self.__Scaling(x_train, x_test)
				## gridsearch ##
				svc = svm.SVC()
				clf = GridSearchCV(svc, self.grid_parameters, cv=4)
				if self.stdout:
					spinner = Halo(text='Parameter optimization by k-fold cross validation', spinner='line')
					spinner.start()
				clf.fit(x_train, t_train)
				if self.stdout:
					spinner.stop()
					print("+ Parameter optimization by k-fold cross validation")
				t_true, t_pred = t_test, clf.predict(x_test)
				## 結果の出力 ##
				if self.stdout:
					print("True labels:")
					print(t_true)
					print("Predict labels:")
					print(t_pred)
					print(f"best parameters : {clf.best_params_}")
					print(f"train best score : {clf.best_score_}")
					print("predict score : {0}".format(metrics.accuracy_score(t_true, t_pred)), end="\n\n")
				## for spread sheet ##
				accuracy_dict[family].append(metrics.accuracy_score(t_true, t_pred)*100.0)
				recall_dict[family].append(recall_score(t_true, t_pred))
				precision_dict[family].append(precision_score(t_true, t_pred))
				## Generate key for json output ##
				train_keys = [str(i) for i in range(1, len(b_train)+1)]
				result_keys = [str(i) for i in range(1, len(b_test)+1)]
				## 再現バッファ出力(スケーリング処理後データを格納) ##
				buffer_dict['CV_'+str(k_count+1)]['train_sample'] = dict(zip(train_keys, b_train))
				buffer_dict['CV_'+str(k_count+1)]['train_label'] = dict(zip(train_keys, self.__ToStringList(t_train)))
				#buffer_dict['CV_'+str(k_count+1)]['train_data'] = self.__GenerateXData(x_train)
				buffer_dict['CV_'+str(k_count+1)]['test_sample'] = dict(zip(result_keys, b_test))
				buffer_dict['CV_'+str(k_count+1)]['test_label'] = dict(zip(result_keys, self.__ToStringList(t_true)))
				#buffer_dict['CV_'+str(k_count+1)]['test_data'] = self.__GenerateXData(x_test)
				## json 出力 ##
				result_dict['CV_'+str(k_count+1)]['sample'] = dict(zip(result_keys, b_test))
				result_dict['CV_'+str(k_count+1)]['true_label'] = dict(zip(result_keys, self.__ToStringList(t_true)))
				result_dict['CV_'+str(k_count+1)]['predict_label'] = dict(zip(result_keys, self.__ToStringList(t_pred)))
				result_dict['CV_'+str(k_count+1)]['best_parameter'] = clf.best_params_
				result_dict['CV_'+str(k_count+1)]['train_best_score'] = clf.best_score_
				result_dict['CV_'+str(k_count+1)]['predict_score'] = metrics.accuracy_score(t_true, t_pred)
				result_dict['CV_'+str(k_count+1)]['recall_score'] = recall_score(t_true, t_pred)
				result_dict['CV_'+str(k_count+1)]['precision_score'] = precision_score(t_true, t_pred)
			json.dump(result_dict, json_f, indent=4)
			json.dump(buffer_dict, b_json_f, indent=4)
			b_json_f.close()
			json_f.close()

		## Spreadsheet 作成 ##
		self.__MakeSpreadSheet(accuracy_dict, path=spread_path+"accuracy_svm.xlsx")
		self.__MakeSpreadSheet(recall_dict, path=spread_path+"recall_svm.xlsx")
		self.__MakeSpreadSheet(precision_dict, path=spread_path+"precision_svm.xlsx")

		## 必要に応じてaccuracyの値を取得 ##
		buf = 0.0
		for family in accuracy_dict.keys():
			buf += float(sum(accuracy_dict[family])/len(accuracy_dict[family]))
		self.accuracy_avg = float(buf / len(accuracy_dict.keys()))

		if self.stdout:
			print("Collect!")


	def MachineLearningRfExtend(self, df):
		## 32次元での2値分類の実験を行うメソッド(ランダムフォレスト) ##
		if self.stdout:
			print("Classification by RandomForest (binary extend)...")
		result_path = self.result_path + "RF/"
		spread_path = self.spread_path + "RF/"
		if os.path.exists(result_path):
			if self.stdout:
				print(f"Delete folder : {result_path}")
			shutil.rmtree(result_path)
		os.makedirs(result_path)
		buffer_path = self.buffer_path + "RF/"
		if os.path.exists(buffer_path):
			if self.stdout:
				print(f"Delete folder : {buffer_path}")
			shutil.rmtree(buffer_path)
		os.makedirs(buffer_path)
		if os.path.exists(spread_path):
			if self.stdout:
				print(f"Delete folder : {spread_path}")
			shutil.rmtree(spread_path)
		os.makedirs(spread_path)
		if self.stdout:
			print(f"The result file will be saved : {result_path}")
			print(f"The buffer file will be saved : {buffer_path}", end="\n\n")
		## Check工程は省いたので自分で確認しましょう ##

		## 結果保存用 ##
		accuracy_dict = {}
		recall_dict = {}
		precision_dict = {}

		## 対象ファミリ毎の実験 ##
		family_length = len(df.groupby('target_family').groups)
		for ite, family in enumerate(df.groupby('target_family').groups):
			if self.stdout:
				print(f"target family : {family} ({ite+1}/{family_length})")
			## 再現用バッファ ##
			buffer_dict = {}
			b_json_file_name = buffer_path + family + ".json"
			b_json_f = open(b_json_file_name, "w")
			## 結果保存用 ##
			result_dict = {}
			json_file_name = result_path + family + ".json"
			json_f = open(json_file_name, "w")
			accuracy_dict[family] = []
			recall_dict[family] = []
			precision_dict[family] = []

			## 分割交差検証用にk個に分ける ##
			x_all_dict, t_all_dict, b_all_dict = self.__PreprocessForClf(df.groupby('target_family').get_group(family))

			## k-fold cross validation ##
			for k_count in range(self.k):
				if self.stdout:
					print(f"k-fold cross validation : ({k_count+1}/{self.k})...")
				buffer_dict['CV_'+str(k_count+1)] = {}
				result_dict['CV_'+str(k_count+1)] = {}
				## train test split ##
				x_train, t_train, b_train = [], [], []
				for key in b_all_dict.keys():
					if key == k_count:
						x_test, t_test, b_test = x_all_dict[key], t_all_dict[key], b_all_dict[key]
					else:
						x_train.extend(x_all_dict[key])
						t_train.extend(t_all_dict[key])
						b_train.extend(b_all_dict[key])
				## 学習データの依存関係保持シャッフル ##
				x_train, t_train, b_train = self.__ShuffleKeepDepend(x_train, t_train, b_train)
				## ndarray化 ##
				x_train, t_train = self.__ToNumpyForClf(x_train, t_train)
				x_test, t_test = self.__ToNumpyForClf(x_test, t_test)
				## スケーリング ##
				if self.scaling:
					x_train, x_test = self.__Scaling(x_train, x_test)
				## 学習 ##
				model = RandomForestClassifier(n_estimators=5000, max_features="sqrt")
				if self.stdout:
					spinner = Halo(text='Parameter optimization by k-fold cross validation', spinner='line')
					spinner.start()
				model.fit(x_train, t_train)
				if self.stdout:
					spinner.stop()
					print("+ Parameter optimization by k-fold cross validation")
				t_true, t_pred = t_test, model.predict(x_test)
				## 結果の出力 ##
				if self.stdout:
					print("True labels:")
					print(t_true)
					print("Predict labels:")
					print(t_pred)
					#print(f"best parameters : {model.best_params_}")
					#print(f"train best score : {model.best_score_}")
					print("predict score : {0}".format(metrics.accuracy_score(t_true, t_pred)), end="\n\n")
				## for spread sheet ##
				accuracy_dict[family].append(metrics.accuracy_score(t_true, t_pred)*100.0)
				recall_dict[family].append(recall_score(t_true, t_pred))
				precision_dict[family].append(precision_score(t_true, t_pred))
				## Generate key for json output ##
				train_keys = [str(i) for i in range(1, len(b_train)+1)]
				result_keys = [str(i) for i in range(1, len(b_test)+1)]
				## 再現バッファ出力(スケーリング処理後データを格納) ##
				buffer_dict['CV_'+str(k_count+1)]['train_sample'] = dict(zip(train_keys, b_train))
				buffer_dict['CV_'+str(k_count+1)]['train_label'] = dict(zip(train_keys, self.__ToStringList(t_train)))
				buffer_dict['CV_'+str(k_count+1)]['train_data'] = self.__GenerateXData(x_train)
				buffer_dict['CV_'+str(k_count+1)]['test_sample'] = dict(zip(result_keys, b_test))
				buffer_dict['CV_'+str(k_count+1)]['test_label'] = dict(zip(result_keys, self.__ToStringList(t_true)))
				buffer_dict['CV_'+str(k_count+1)]['test_data'] = self.__GenerateXData(x_test)
				## json 出力 ##
				result_dict['CV_'+str(k_count+1)]['sample'] = dict(zip(result_keys, b_test))
				result_dict['CV_'+str(k_count+1)]['true_label'] = dict(zip(result_keys, self.__ToStringList(t_true)))
				result_dict['CV_'+str(k_count+1)]['predict_label'] = dict(zip(result_keys, self.__ToStringList(t_pred)))
				result_dict['CV_'+str(k_count+1)]['best_parameter'] = None
				result_dict['CV_'+str(k_count+1)]['train_best_score'] = None
				result_dict['CV_'+str(k_count+1)]['predict_score'] = metrics.accuracy_score(t_true, t_pred)
				result_dict['CV_'+str(k_count+1)]['recall_score'] = recall_score(t_true, t_pred)
				result_dict['CV_'+str(k_count+1)]['precision_score'] = precision_score(t_true, t_pred)
			json.dump(result_dict, json_f, indent=4)
			json.dump(buffer_dict, b_json_f, indent=4)
			b_json_f.close()
			json_f.close()
		## Spreadsheet 作成 ##
		self.__MakeSpreadSheet(accuracy_dict, path=spread_path+"accuracy_rf.xlsx")
		self.__MakeSpreadSheet(recall_dict, path=spread_path+"recall_rf.xlsx")
		self.__MakeSpreadSheet(precision_dict, path=spread_path+"precision_rf.xlsx")

		## 必要に応じてaccuracyの値を取得 ##
		buf = 0.0
		for family in accuracy_dict.keys():
			buf += float(sum(accuracy_dict[family])/len(accuracy_dict[family]))
		self.accuracy_avg = float(buf / len(accuracy_dict.keys()))

		if self.stdout:
			print("Collect!")

	def GetAccuracyAverage(self):
		"""
		正解率の平均値を取得
		"""
		if self.accuracy_avg == -1.0:
			print("No Data : accuracy average")
		else:
			return self.accuracy_avg


	## privates ##
	def __CheckOnce(self):
		"""
		2回以上の学習は別のインスタンスを作成する必要がある
		"""
		if self.accuracy_avg != -1.0:
			print("2回以上の学習は別のインスタンスを作成する必要があります")
			sys.exit(1)

	def __GenerateXData(self, x):
		"""
		x_trainを辞書型に再構成
		buffer出力用
		可視性のため関数化
		"""
		dictionary = {}
		for index, items in enumerate(x):
			dictionary[str(index+1)] = {"targetCR":str(items[0]), "otherCR":str(items[1])}
		return dictionary

	def __PreprocessForClf(self, df):
		"""
		DataFrameから学習・テストデータとラベルを生成(k-fold-crossvalidation)
		x:学習・テストデータ
		t:学習・テストラベル
		b:検体情報の順序保持
		"""
		x, t, b = {}, {}, {}
		df_target = df.groupby('label').get_group('target')
		df_other = df.groupby('label').get_group('other')
		## 正例 ##
		for index, data in zip(range(len(df_target)), df_target.itertuples()):
			key = index % self.k
			if key not in x.keys():
				x[key], t[key], b[key] = [], [], []
			## 規定通りなら5個目以降のデータが圧縮率 ##
			x[key].append(list(data[5:]))
			t[key].append(1)
			tmp = data.sample_file_path.split("/")
			b[key].append(tmp[-2] + "_" + tmp[-1])
		## 負例 ##
		for index, data in zip(range(len(df_other)), df_other.itertuples()):
			key = index % self.k
			if key not in x.keys():
				print("Error : Possibility that the allocation of the positive example has failed.")
				sys.exit(1)
			x[key].append(list(data[5:]))
			t[key].append(0)
			tmp = data.sample_file_path.split("/")
			b[key].append(tmp[-2] + "_" + tmp[-1])
		return x, t, b

	def __ToNumpyForClf(self, train, test):
		"""
		ndarrayにして返す
		第一引数にfloat32型データ，第二引数に教師データを与える
		"""
		r_train = numpy.array(train).astype('float32')
		r_test = numpy.array(test)
		return r_train, r_test

	def __ShuffleKeepDepend(self, x, t, b):
		"""
		3リストの依存関係を保持したままシャッフルする
		x:学習
		"""
		all_list = list(zip(x, t, b))
		random.shuffle(all_list)
		x, t, b = zip(*all_list)
		return x, t, b

	def __Scaling(self, train, test):
		"""
		学習用データでスケーリングを行い，その時のパラメータで推論データをスケーリングする．
		ここでは，スケーリングとして標準化を用いる．
		"""
		scaler = preprocessing.StandardScaler()
		scaler.fit(train)
		train = scaler.transform(train)
		test = scaler.transform(test)
		return train, test

	def __ToStringList(self, l):
		"""
		リストの要素を文字列に変換
		l : リスト
		"""
		return [str(item) for item in l]

	def __MakeSpreadSheet(self, accuracy_dict, path="./accuracy.xlsx"):
		"""
		family毎に記録されたaccuracy_dictに則り，
		excelシートを作成する
		"""
		if self.stdout:
			print("Make Spread Sheet...")
			print(f"{path}")
		Data = []
		row_index = []
		col_index = ["k="+str(i+1) for i in range(self.k)]
		col_index.append("Avg.")
		for family in accuracy_dict.keys():
			buf = float(sum(accuracy_dict[family])/len(accuracy_dict[family]))
			accuracy_dict[family].append(buf)
			Data.append(accuracy_dict[family])
			row_index.append(family)
		df = pd.DataFrame(Data)
		df.columns = col_index
		df.index = row_index
		df.to_excel(path, float_format='%.2f')
		if self.stdout:
			print("Collect!", end="\n\n")


## main ##

if __name__=='__main__':
	
	dataset = PreprocessDataset(dataset_path="./Dataset/", buffer_path="./Buffer/")

	tree_set = RegistrationFeatureExtractionMultivalued(dataset[0], compress_mode="lzt")

	data_frame = CalculateCompressRatioMultivalued(tree_set, dataset[1], gamma=30)

	machin= ANT_MachineLearning("./Buffer/", "./Result/")

	machin.MachineLearningSvmExtend(data_frame)