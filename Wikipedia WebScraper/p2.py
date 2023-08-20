 
import ply.lex as lex
import ply.yacc as yacc
 
 
 
 
###DEFINING TOKENS###
 
tokens = ('BEGINTABLE', 'IMG','BEGINTEAM',
'OPENTABLE', 'CLOSETABLE', 'OPENROW', 'CLOSEROW',
'OPENHEADER', 'CLOSEHEADER', 'OPENHREF', 'CLOSEHREF',
'CONTENT', 'OPENDATA', 'CLOSEDATA' ,'OPENSPAN',
'CLOSESPAN', 'OPENDIV', 'CLOSEDIV', 'OPENSTYLE', 'CLOSESTYLE','GARBAGE')
t_ignore = '\t'
 
 
###############Tokenizer Rules################
 
def t_BEGINTEAM(t):
 
    '''<table\sclass="wikitable\ssortable\smw-collapsible\smw-collapsed">'''
 
    return t
 

def t_IMG(t):
    '''<img[^>]&>'''
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
    return t
 
def t_CLOSEDIV(t):
    '''</div[^>]*>'''
    return t
 
def t_OPENSTYLE(t):
    '''<style[^>]*>'''
    return t

 
def t_CLOSESTYLE(t):
    '''</style[^>]*>'''
    return t
 
def t_OPENSPAN(t):
    '''<span[^>]*>'''
    return t
 
def t_CLOSESPAN(t):
    '''</span[^>]*>'''
    return t
 
def t_GARBAGE(t):
    r'<[^>]*>'
    # return t
 
def t_error(t):
    t.lexer.skip(1) 
    # return t
 
####################Parsing Rules###############
 
def p_start(p):
    '''start : table'''
    print("start")
 
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
 
def p_skiptag(p):
    '''skiptag : CONTENT skiptag
               | OPENHREF skiptag
               | CLOSEHREF skiptag
               | OPENSPAN skiptag
               | CLOSESPAN skiptag
               | OPENHEADER skiptag
               | CLOSEHEADER skiptag
               | IMG skiptag
               | '''

def p_handleData(p):
    '''handleData : OPENDATA skiptag CLOSEDATA handleData
                  | OPENDATA OPENSPAN OPENSPAN CONTENT CLOSESPAN OPENHREF CONTENT CLOSEHREF CLOSESPAN CLOSEDATA handleData
                  | '''
   

    if len(p) == 12:
        print(p[7])
 
 
def p_handlerow(p):
    '''handlerow : OPENROW OPENHEADER CONTENT skiptag CLOSEROW handlerow
                 | OPENROW handleData CLOSEROW handlerow  
                 |'''
    #print("handlerow")
 
def p_table(p):
    '''table : BEGINTEAM  OPENTABLE  handlerow'''
    #print("p_table")
 
 
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
    #print("hello")
    import ply.lex as lex

    file_obj= open('webpage.html','r',encoding="utf-8")
    data=file_obj.read()
    lexer = lex.lex()
    lexer.input(data)
    f = open("lex_out.txt","w")
    for tok in lexer:
        f.write(str(tok)+" \n ")
        
    parser = yacc.yacc()
    #print("hello3")
    parser.parse(data)
    #print("hello4")
    file_obj.close()
    #f.close()
 
###############################################################################
 
# if __name__ == '__main__':
#     main()