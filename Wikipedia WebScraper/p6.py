 
import ply.lex as lex
import ply.yacc as yacc
 
 
 
global awards
awards=["Bronze Ball","Silver Ball","Golden Ball","Bronze Boot","Silver Boot","Golden Boot"]
global i
i=0
###DEFINING TOKENS###
 
tokens = ('BEGINTABLE', 'IMG',
'OPENTABLE', 'CLOSETABLE', 'OPENROW', 'CLOSEROW',
'OPENHEADER', 'CLOSEHEADER', 'OPENHREF', 'CLOSEHREF',
'CONTENT', 'OPENDATA', 'CLOSEDATA' ,'OPENSPAN',
'CLOSESPAN', 'OPENDIV', 'CLOSEDIV', 'OPENSTYLE', 'CLOSESTYLE','GARBAGE')
t_ignore = '\t'
 
 
###############Tokenizer Rules################
 
def t_BEGINTABLE(t):
 
    '''<table\sclass="wikitable"\sstyle="text-align:center">'''
 
    return t
 
 
def t_OPENTABLE(t):
    '''<tbody[^>]*>'''
    return t
 
def t_CLOSETABLE(t):
    '''</tbody[^>]*>'''
    return t
 
def t_OPENROW(t):
    '''<tr[^>]*>'''
    return t
 
def t_CLOSEROW(t):
    '''</tr[^>]*>'''
    return t
 
def t_OPENHEADER(t):
    '''<th[^>]*>'''
    return t
 
def t_CLOSEHEADER(t):
    '''</th[^>]*>'''
    return t
 
def t_OPENHREF(t):
    '''<a[^>]*>'''
    return t
 
def t_CLOSEHREF(t):
    '''</a[^>]*>'''
    return t
 
def t_OPENDATA(t):
    '''<td[^>]*>'''
    return t
 
def t_CLOSEDATA(t):
    '''</td[^>]*>'''
    return t
 
def t_CONTENT(t):
    '''[A-Za-z0-9, ]+'''
    return t
 
def t_OPENDIV(t):
    '''<div[^>]*>'''
 
def t_CLOSEDIV(t):
    '''</div[^>]*>'''
 
def t_OPENSTYLE(t):
    '''<style[^>]*>'''
 
def t_CLOSESTYLE(t):
    '''</style[^>]*>'''
    
 
def t_OPENSPAN(t):
    '''<span[^>]*>'''
    return t
 
def t_CLOSESPAN(t):
    '''</span[^>]*>'''
    return t

def t_IMG(t):
    '''<img[^>]*>'''
    return t
 
def t_GARBAGE(t):
    r'<[^>]*>'
 
def t_error(t):
    t.lexer.skip(1)
 
 
####################Parsing Rules###############
 
def p_start(p):
    '''start : table'''
 
    p[0] = p[1]
 
def p_name(p):
    '''name : CONTENT
            | CONTENT name'''
    if len(p) == 3:
        #First production is getting evaluated
        p[0] = p[1] + ' ' + p[2]
    else:
        #2nd Production is getting evaluated
        p[0] = p[1]
players=[]
awards=['golden ball']
def p_skiptag(p):
    '''skiptag : OPENHREF skiptag
               | CLOSEHREF skiptag
               | OPENSPAN skiptag
               | CLOSESPAN skiptag
               | IMG skiptag
               | OPENHEADER skiptag
               | CLOSEHEADER skiptag
               |'''


def p_getContent(p):
    '''getContent : CONTENT CONTENT CONTENT getContent
                  | CONTENT CONTENT getContent 
                  | '''
    if(len(p)>=4):
        players.append(p[1])


def p_handleData(p):
    '''handleData : OPENDATA skiptag CONTENT OPENHREF CONTENT CLOSEHREF CLOSEDATA handleData
                  | OPENDATA skiptag  CLOSEDATA CONTENT handleData
                  | OPENDATA CONTENT CONTENT CLOSEDATA handleData
                  | OPENDATA skiptag getContent CLOSEDATA handleData
                  | '''

    if(len(p) == 9 ):
        players.append(p[5])
        
                  
def p_temp_data(p):
    '''temp_data : OPENHEADER CONTENT CLOSEHEADER temp_data
                 | '''
    if(len(p)==5):
        awards.append(p[2])
    
def p_handlerow(p):
    '''handlerow : OPENROW OPENHEADER OPENHREF CONTENT CLOSEHREF CLOSEHEADER temp_data CLOSEROW handlerow
                 | OPENROW OPENHEADER OPENHREF CONTENT skiptag CLOSEROW handleData
                 | OPENROW handleData CLOSEROW handlerow
                 | '''
    if(len(p)>=8):
        awards.append(p[4])
        
    

def p_table(p):
    '''table : BEGINTABLE OPENTABLE handlerow'''
 
 
def p_empty(p):
    '''empty :'''
    pass
 
def p_content(p):
    '''content : CONTENT
               | empty'''
    p[0] = p[1]
 
def p_error(p):
    pass
 
#########DRIVER FUNCTION#######
 
def run():
    file_obj= open('webpage.html','r',encoding="utf-8")
    data=file_obj.read()
    lexer = lex.lex()
    lexer.input(data)
    f = open("lex_out.txt","w")
    for tok in lexer:
        f.write(str(tok)+" \n ")
    parser = yacc.yacc()
    parser.parse(data)
    file_obj.close()
    print(players)
    print(awards)
###############################################################################
 
# if __name__ == '__main__':
#     main()