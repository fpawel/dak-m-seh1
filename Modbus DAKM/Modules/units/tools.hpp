// Содержит ли последовательность container число val
bool IsValueIn(variant val, variant container)
{		
    int sz =ArraySize(container);
    for( int i=0; i<sz; ++i )
        if(val==container[i] )        
            return true;        
    return false;
}
//------------------------------------------------------------------------------------
variant Key2Val(key, def, map)
{
	int sz =ArraySize( map[0] );
	for( int i=0; i<sz; ++i )
        if(map[0][i]==key )        
            return map[1][i];        
    return def;
}
//------------------------------------------------------------------------------------