#!/usr/bin/python
FILE_ADDRESS = "META_FILE.list"
def make_filedic():
    user_dic = {}
    try:
        FD = open(FILE_ADDRESS, "r")
    except:
        return user_dic

    for line in FD.readlines():
        block = line.rstrip("\n").split("\t")
        try:
            tmp_set = user_dic[block[0]]
            user_dic[block[0]] = tmp_set.add(block[1])
        except KeyError:
            tmp_set = set()
            user_dic[block[0]] = tmp_set.add(block[1])
    FD.close()
    return user_dic


def exist(user_name, filename):
    user_file_dic = make_filedic()
    try:
        tmp_set = user_file_dic[user_name]
        if filename in tmp_set:
            return 1
    except KeyError:
        return 0
    return 0


def write(user_name, filename):
    if not exist(user_name, filename):
        try:
            FD = open(FILE_ADDRESS, "a")
        except:
            FD = open(FILE_ADDRESS, "w")
        print >> FD, user_name+"\t"+filename
        FD.close()
    return

def list(user_name):
    user_file_dic = make_filedic()
    tmp_stream = ""
    tmp_set = user_file_dic[user_name]
    j = 0
    for i in tmp_set:
        if j%5==0:
            delim = "\n"
        else:
            delim = "\t"
        tmp_stream += delim + i
        j +=1
    return tmp_stream
