#include <u.h>
#include <libc.h>
#include <ctype.h>

enum {
	NUM = 1,
	ANUM = 2,
	BYTE = 4,
};

static uchar gpexp[255] =
{1, 2, 4, 8, 16, 32, 64, 128, 29, 58, 116, 232, 205, 135, 19, 38, 76, 
152, 45, 90, 180, 117, 234, 201, 143, 3, 6, 12, 24, 48, 96, 192, 157, 
39, 78, 156, 37, 74, 148, 53, 106, 212, 181, 119, 238, 193, 159, 35, 
70, 140, 5, 10, 20, 40, 80, 160, 93, 186, 105, 210, 185, 111, 222, 
161, 95, 190, 97, 194, 153, 47, 94, 188, 101, 202, 137, 15, 30, 60, 
120, 240, 253, 231, 211, 187, 107, 214, 177, 127, 254, 225, 223, 163, 
91, 182, 113, 226, 217, 175, 67, 134, 17, 34, 68, 136, 13, 26, 52, 
104, 208, 189, 103, 206, 129, 31, 62, 124, 248, 237, 199, 147, 59, 
118, 236, 197, 151, 51, 102, 204, 133, 23, 46, 92, 184, 109, 218, 
169, 79, 158, 33, 66, 132, 21, 42, 84, 168, 77, 154, 41, 82, 164, 85, 
170, 73, 146, 57, 114, 228, 213, 183, 115, 230, 209, 191, 99, 198, 
145, 63, 126, 252, 229, 215, 179, 123, 246, 241, 255, 227, 219, 171, 
75, 150, 49, 98, 196, 149, 55, 110, 220, 165, 87, 174, 65, 130, 25, 
50, 100, 200, 141, 7, 14, 28, 56, 112, 224, 221, 167, 83, 166, 81, 
162, 89, 178, 121, 242, 249, 239, 195, 155, 43, 86, 172, 69, 138, 9, 
18, 36, 72, 144, 61, 122, 244, 245, 247, 243, 251, 235, 203, 139, 11, 
22, 44, 88, 176, 125, 250, 233, 207, 131, 27, 54, 108, 216, 173, 71, 
142};

static uchar gplog[256] = 
{0, 0, 1, 25, 2, 50, 26, 198, 3, 223, 51, 238, 27, 104, 199, 75, 4, 100, 
224, 14, 52, 141, 239, 129, 28, 193, 105, 248, 200, 8, 76, 113, 5, 
138, 101, 47, 225, 36, 15, 33, 53, 147, 142, 218, 240, 18, 130, 69, 
29, 181, 194, 125, 106, 39, 249, 185, 201, 154, 9, 120, 77, 228, 114, 
166, 6, 191, 139, 98, 102, 221, 48, 253, 226, 152, 37, 179, 16, 145, 
34, 136, 54, 208, 148, 206, 143, 150, 219, 189, 241, 210, 19, 92, 
131, 56, 70, 64, 30, 66, 182, 163, 195, 72, 126, 110, 107, 58, 40, 
84, 250, 133, 186, 61, 202, 94, 155, 159, 10, 21, 121, 43, 78, 212, 
229, 172, 115, 243, 167, 87, 7, 112, 192, 247, 140, 128, 99, 13, 103, 
74, 222, 237, 49, 197, 254, 24, 227, 165, 153, 119, 38, 184, 180, 
124, 17, 68, 146, 217, 35, 32, 137, 46, 55, 63, 209, 91, 149, 188, 
207, 205, 144, 135, 151, 178, 220, 252, 190, 97, 242, 86, 211, 171, 
20, 42, 93, 158, 132, 60, 57, 83, 71, 109, 65, 162, 31, 45, 67, 216, 
183, 123, 164, 118, 196, 23, 73, 236, 127, 12, 111, 246, 108, 161, 
59, 82, 41, 157, 85, 170, 251, 96, 134, 177, 187, 204, 62, 90, 203, 
89, 95, 176, 156, 169, 160, 81, 11, 245, 22, 235, 122, 117, 44, 215, 
79, 174, 213, 233, 230, 231, 173, 232, 116, 214, 244, 234, 168, 80, 
88, 175};

static uchar *polys[69];
static int cap[160];
static uchar *blocks[160];

static uchar side[40] = {
	21, 25, 29, 33, 37, 41, 45, 49,
	53, 57, 61, 65, 69, 73, 77, 81,
	85, 89, 93, 97, 101, 105, 109, 113,
	117, 121, 125, 129, 133, 137, 141, 145,
	149, 153, 157, 161, 165, 169, 173, 177
};

static uchar align[40][8] = {
	{0},
	{6, 18, 0},
	{6, 22, 0},
	{6, 26, 0},
	{6, 30, 0},
	{6, 34, 0},
	{6, 22, 38, 0},
	{6, 24, 42, 0},
	{6, 26, 46, 0},
	{6, 28, 50, 0},
	{6, 30, 54, 0},
	{6, 32, 58, 0},
	{6, 34, 62, 0},
	{6, 26, 46, 66, 0},
	{6, 26, 48, 70, 0},
	{6, 26, 50, 74, 0},
	{6, 30, 54, 78, 0},
	{6, 30, 56, 82, 0},
	{6, 30, 58, 86, 0},
	{6, 34, 62, 90, 0},
	{6, 28, 50, 72, 94, 0},
	{6, 26, 50, 74, 98, 0},
	{6, 30, 54, 78, 102, 0},
	{6, 28, 54, 80, 106, 0},
	{6, 32, 58, 84, 110, 0},
	{6, 30, 58, 86, 114, 0},
	{6, 34, 62, 90, 118, 0},
	{6, 26, 50, 74, 98, 122, 0},
	{6, 30, 54, 78, 102, 126, 0},
	{6, 26, 52, 78, 104, 130, 0},
	{6, 30, 56, 82, 108, 134, 0},
	{6, 34, 60, 86, 112, 138, 0},
	{6, 30, 58, 86, 114, 142, 0},
	{6, 34, 62, 90, 118, 146, 0},
	{6, 30, 54, 78, 102, 126, 150, 0},
	{6, 24, 50, 76, 102, 128, 154, 0},
	{6, 28, 54, 80, 106, 132, 158, 0},
	{6, 32, 58, 84, 110, 136, 162, 0},
	{6, 26, 54, 82, 110, 138, 166, 0},
	{6, 30, 58, 86, 114, 142, 170, 0},
};

static void
qrinit(void)
{
	uchar *p;
	int in;
	extern uchar qrrawpolys[];
	extern uchar qrsizes[];
	
	in = 0;
	for(p = qrrawpolys; in || *p != 0; p++){
		if(!in)
			polys[*p] = p + 1;
		in = *p;
	}
	memset(cap, 0, sizeof(cap));
	for(p = qrsizes; p[0] != 0 || p[1] != 0; p += 4){
		cap[p[0]] += p[1] * p[3];
		if(blocks[p[0]] == nil)
			blocks[p[0]] = p;
	}
}

static void
bits(uchar *b, int *elen, int *bctr, ulong data, int n)
{
	int l, bc, s;
	
	l = *elen;
	bc = *bctr;
	
	s = b[l] << n | data;
	bc += n;
	while(bc > 8)
		b[l++] = s >> (bc -= 8);
	b[l] = s;
	if(bc == 8){
		l++;
		bc = 0;
	}
	
	*elen = l;
	*bctr = bc;
}

static int
getlbits(int version, int mode)
{
	if(version <= 9)
		switch(mode){
		case NUM: return 10;
		case ANUM: return 9;
		case BYTE: return 8;
		}
	else if(version <= 26)
		switch(mode){
		case NUM: return 12;
		case ANUM: return 11;
		case BYTE: return 16;
		}
	else
		switch(mode){
		case NUM: return 14;
		case ANUM: return 13;
		case BYTE: return 16;
		}
	werrstr("unknown mode %#x", mode);
	return -1;
}

static char
atab[] = {
	['0'] 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
	['A'] 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
	26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
	['a'] 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
	26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
	[' '] 37, ['$'] 38, ['%'] 39, ['*'] 40, ['+'] 41, ['-'] 42, 
	['.'] 43, ['/'] 44, [':'] 45 
};

static uchar
formsel(int version, int level, int mode, uchar *data, int dlen, int *nchar)
{
	int bits, lbits, i, n;
	
	bits = 8;
	switch(mode){
	case NUM:
		n = 0;
		for(i = 0; i < dlen; i++)
			if(isdigit(data[i]))
				n++;
		*nchar = n;
		bits += 10 * (n / 3);
		switch(n%3){
		case 1: bits += 4; break;
		case 2: bits += 7; break;
		}
		break;
	case ANUM:
		n = 0;
		for(i = 0; i < dlen; i++)
			if(atab[data[i]] != 0)
				n++;
		*nchar = n;
		bits += 11 * (n >> 1) + 6 * (n & 1);
		break;
	case BYTE:
		bits += 8 * dlen;
		*nchar = dlen;
		break;
	}
	if(version == 0){
		SET(lbits);
		for(i = 0; i < 40; i++){
			if(i == 0 || i == 10 || i == 27){
				lbits = getlbits(i + 1, mode);
				if(lbits < 0)
					return -1;
			}
			if(cap[i << 2 | level] >= (bits + lbits + 7 >> 3))
				return i << 2 | level;
		}
	}else{
		lbits = getlbits(version, mode);
		if(lbits < 0)
			return -1;
		if(cap[version - 1 << 2 | level] >= (bits + lbits + 7 >> 3))
			return version - 1 << 2 | level;
	}
	werrstr("too many bits");
	return -1;
}

static uchar *
encode(uchar verlev, uchar *data, int dlen, int mode, int nchar)
{
	uchar *b;
	int bc, n, c, nd;
	int acc;
	
	n = 0;
	c = cap[verlev];
	b = malloc(c);
	if(b == nil)
		return nil;
	bits(b, &n, &bc, mode, 4);
	bits(b, &n, &bc, nchar, getlbits((verlev >> 2) + 1, mode));
	switch(mode){
	case NUM:
		nd = 0;
		acc = 0;
		for(; dlen-- != 0; data++)
			if(isdigit(*data)){
				acc = acc * 10 + *data - '0';
				if(++nd == 3){
					nd = 0;
					bits(b, &n, &bc, acc, 10);
					acc = 0;
				}
			}
		if(nd == 1)
			bits(b, &n, &bc, acc, 4);
		else if(nd == 2)
			bits(b, &n, &bc, acc, 7);
		break;
	case ANUM:
		nd = 0;
		acc = 0;
		for(; dlen-- != 0; data++)
			if(atab[*data] != 0){
				acc = acc * 45 + atab[*data] - 1;
				if(++nd == 2){
					nd = 0;
					bits(b, &n, &bc, acc, 11);
					acc = 0;
				}
			}
		if(nd == 1)
			bits(b, &n, &bc, acc, 6);
		break;
	case BYTE:
		while(dlen-- != 0)
			bits(b, &n, &bc, *data++, 8);
		break;
	default:
		werrstr("unknown mode %#x", mode);
		return nil;
	}
	bits(b, &n, &bc, 0, 4);
	if(bc != 0)
		b[n++] <<= 8 - bc;
	while(n < c){
		b[n++] = 0xec;
		if(n == c)
			break;
		b[n++] = 0x11;
	}
	return b;
}

/* all stored highest coefficient first
   poly stored as log and highest coefficient implicit 1 */
static int
ecc(uchar *data, int dl, uchar *poly, uchar *err, int el, int str)
{
	uchar *q;
	uchar *dp;
	uchar l;
	int i, x;
	
	q = malloc(el);
	if(q == nil)
		return -1;
	for(i = 0; i < el; i++){
		q[i] = gpexp[poly[i]];
		err[i * str] = 0;
	}
	for(dp = data + dl - 1; dp != data - 1; dp--){
		if(*dp != 0){
			l = gplog[*dp];
			for(i = 0; i < el; i++){
				if(q[i] != 0){
					x = gplog[q[i]] + l;
					if(x >= 255)
						x -= 255;
					err[i * str] ^= gpexp[x];
				}
			}
		}
		l = q[0];
		if(l != 0){
			l = gplog[l];
			for(i = 0; i < el; i++){
				x = l + poly[i];
				if(x >= 255)
					x -= 255;
				q[i] = (i == el-1 ? 0 : q[i+1]) ^ gpexp[x];
			}
		}else{
			for(i = 0; i < el - 1; i++)
				q[i] = q[i+1];
			q[el - 1] = 0;
		}
	}
	free(q);
	return 0;
}

static uchar *
codewords(uchar verlev, uchar *data, int *ncp)
{
	uchar *bp, *p, *dp, *r, *rp;
	uchar *ep;
	int ne, i, j, el, nc, nb, ncol;
	
	bp = blocks[verlev];
	el = bp[2] - bp[3]; /* guaranteed constant */
	ncol = 0;
	for(p = bp, ne = 0, nc = 0, nb = 0; p[0] == verlev; p += 4){
		nb += p[1];
		ne += p[1] * el;
		nc += p[1] * p[2];
		ncol = p[3];
	}
	r = malloc(nc);
	if(r == nil)
		return nil;
	for(p = bp, dp = data, ep = r + nc - ne; p[0] == verlev; p += 4)
		for(j = 0; j < p[1]; j++){
			if(ecc(dp, p[3], polys[el], ep, el, nb) < 0){
				free(r);
				return nil;
			}
			dp += p[3];
			ep++;
		}
	for(i = 0, rp = r; i < ncol; i++){
		dp = data + i;
		for(p = bp; p[0] == verlev; p += 4){
			for(j = 0; j < p[1]; j++){
				if(p[3] > i)
					*rp++ = *dp;
				dp += p[3];
			}
		}
	}
	*ncp = nc;
	return r;
}

#define max(i,j) ((i)>(j)?(i):(j))

uchar *
basepat(int verlev)
{
	int s, i, j, d;
	uchar *pat, *p, *ap, *aq;
	
	s = side[verlev >> 2];
	pat = malloc((s + 8) * (s + 8));
	if(pat == nil)
		return nil;
	p = pat;
	for(j = -4; j < s + 4; j++)
		for(i = -4; i < s + 4; i++){
			if(i < 0 || j < 0 || i >= s || j >= s)
				*p++ = 0xff;
			else if(i < 8 && j < 8 || i >= s - 8 && j < 8 || i < 8 && j >= s - 8){
				d = max(abs(i >= 8 ? i - s + 4 : i-3), abs(j >= 8 ? j - s + 4 : j-3));
				*p++ = d == 2 || d == 4 ? 0xff : 0;
			}else if(i == 6 || j == 6)
				*p++ = (i + j & 1) != 0 ? 0xff : 0;
			else if(i <= 8 && j <= 8 || i <= 8 && j >= s - 8 || i >= s - 8 && j <= 8)
				*p++ = 0xd0;
			else if(verlev >= 24 && (j >= s - 11 && i < 6 || i >= s - 11 && j < 6))
				*p++ = 0xc0;
			else
				*p++ = 0x80;
		}
	for(ap = align[verlev >> 2]; *ap != 0; ap++)
		for(aq = align[verlev >> 2]; *aq != 0; aq++){
			if(*ap == 6 && *aq == 6 || *ap >= s - 7 && *aq == 6 || *ap == 6 && *aq >= s - 7)
				continue;
			for(i = *ap - 2; i <= *ap + 2; i++)
				for(j = *aq - 2; j <= *aq + 2; j++)
					pat[i * (s + 8) + j + 4 + (s + 8) * 4] = max(abs(i - *ap), abs(j - *aq)) == 1 ? 0xff : 0;
		}
	return pat;
}

static void
fill(uchar verlev, uchar *p, uchar *d, int nc)
{
	int s, x, y, i, up;
	uchar *dp, db;
	
	s = side[verlev >> 2] + 8;
	x = s - 5;
	y = s - 5;
	up = 1;
	SET(db);
	for(dp = d, i = 0;;){
		if(i == 0){
			if(dp - d == nc)
				db = 0;
			else
				db = *dp++;
		}
		i = i + 1 & 7;
		while(p[y * s + x] != 0x80){
			if(up){
				if((x & 1 ^ (x < 10)) == 0)
					x--;
				else if(y >= 4){
					x++;
					y--;
				}else{
					x--;
					up = 0;
					if(x == 10)
						x--;
				}
			}else{
				if((x & 1 ^ (x < 10)) == 0)
					x--;
				else if(y < s - 4){
					x++;
					y++;
				}else{
					x--;
					up = 1;
					if(x == 10)
						x--;
				}
			}
			if(x < 0)
				return;
		}
		p[y * s + x] = ~((char)db >> 7) ^ 0x0f;
		db <<= 1;
	}
}

static void
mask(uchar verlev, uchar *p, int m)
{
	uchar *q;
	int s, i, j;
	
	s = side[verlev >> 2];
	for(i = -4, q = p; i < s + 4; i++)
		for(j = -4; j < s + 4; j++, q++)
			if(((*q ^ *q >> 4) & 15) == 15)
				switch(m){
				case 0: *q ^= (char)((i+j+1) << 7) >> 7; break;
				case 1: *q ^= (char)((i+1) << 7) >> 7; break;
				case 2: *q ^= (char)(j % 3 - 1) >> 7; break;
				case 3: *q ^= (char)((i+j) % 3 - 1) >> 7; break;
				case 4: *q ^= (char)(((i/2)+(j/3)+1) << 7) >> 7; break;
				case 5: *q ^= (char)((i*j%2)+(i*j%3) - 1) >> 7; break;
				case 6: *q ^= (char)((i*j+(i*j%3)+1) << 7) >> 7; break;
				case 7: *q ^= (char)((i+j+(i*j%3)+1) << 7) >> 7; break;
				}
}

static int
evaluate(uchar verlev, uchar *p)
{
	int r, i, j, l, s, sc, cnt;
	uchar q, sh;
	uchar *pp;
	enum {
		N1 = 3,
		N2 = 3,
		N3 = 40,
		N4 = 10,
	};
	
	s = side[verlev >> 2];
	sc = 0;
	cnt = 0;

	for(i = 0; i < s; i++){
		r = 0;
		l = -1;
		sh = 0;
		for(j = 0; j < s; j++){
			q = p[i * (s + 8) + j + 4 + (s + 8) * 4] >= 0x80;
			cnt += q;
			sh = sh << 1 | l;
			if(q != l){
				if(r >= 5)
					sc += N1 + r;
				l = q;
				r = 0;
			}
			if((sh & 0x7f) == 0x5d)
				sc += N3;
		}
		if(r >= 5)
			sc += N1 * r;
	}
	sc = abs(cnt - s * s / 2) / (s * s / 20) * N4;
	
	for(j = 0; j < s; j++){
		r = 0;
		l = -1;
		sh = 0;
		for(i = 0; i < s; i++){
			q = p[i * (s + 8) + j + 4 + (s + 8) * 4];
			sh = sh << 1 | l;
			if(q != l){
				if(r >= 5)
					sc += N1 + r;
				l = q;
				r = 0;
			}
			if((sh & 0x7f) == 0x5d)
				sc += N3;
		}
		if(r >= 5)
			sc += N1 * r;
	}

	/* approximation */
	for(j = 1; j < s; j++)
		for(i = 1; i < s; i++){
			pp = p + i * (s + 8) + j + 4 + (s + 8) * 4;
			if(pp[0] == pp[-1] && pp[0] == pp[-(s+8)] && pp[0] == pp[-1-(s+8)])
				sc += N2;
		}
	return sc;
}

static void
format(uchar verlev, uchar *p, int m)
{
	static ushort format[] = 
		{21522, 20773, 24188, 23371, 17913, 16590, 20375, 19104, 30660, 
		29427, 32170, 30877, 26159, 25368, 27713, 26998, 5769, 5054, 7399, 
		6608, 1890, 597, 3340, 2107, 13663, 12392, 16177, 14854, 9396, 8579, 
		11994, 11245};
	int s, i;
	ushort f;
	uchar c;
	
	s = side[verlev >> 2];
	f = ~format[verlev << 3 & 0x18 ^ 8 | m];
	for(i = 0; i < 15; i++){
		c = (char)(f << 7) >> 7;
		if(i < 8){
			p[(i + (i >= 6)) * (s + 8) + 8 + 4 + (s + 8) * 4] = c;
			p[12 * (s + 8) + (s + 3 - i)] = c;
		}else{
			p[(s - 15 + i) * (s + 8) + 8 + 4 + (s + 8) * 4] = c;
			p[12 * (s + 8) + (18 - i + (i == 8))] = c;
		}
		f >>= 1;
	}
	p[(s - 8) * (s + 8) + 8 + 4 + (s + 8) * 4] = 0;
}

static void
version(uchar verlev, uchar *p)
{
	int s, i, f, q;
	uchar c;
	static int version[] = {
		0x07C94, 0x085BC, 0x09A99, 0x0A4D3, 0x0BBF6, 0x0C762, 0x0D847,
		0x0E60D, 0x0F928, 0x10B78, 0x1145D, 0x12A17, 0x13532, 0x149A6,
		0x15683, 0x168C9, 0x177EC, 0x18EC4, 0x191E1, 0x1AFAB, 0x1B08E,
		0x1CC1A, 0x1D33F, 0x1ED75, 0x1F250, 0x209D5, 0x216F0, 0x228BA,
		0x2379F, 0x24B0B, 0x2542E, 0x26A64, 0x27541, 0x28C69
	};
	
	if(verlev < 24)
		return;
	s = side[verlev >> 2] + 8;
	f = ~version[(verlev >> 2) - 6];
	for(i = 0; i < 18; i++){
		c = (char)(f << 7) >> 7;
		f >>= 1;
		q = (s - 15 + i % 3) * s + (4 + i / 3);
		p[q] = c;
		p[(q * s) % (s * s - 1)] = c;
	}
}

static void
finalize(uchar verlev, uchar *p)
{
	int s;
	
	s = side[verlev >> 2] + 8;
	s *= s;
	while(s--)
		*((char *)p++) >>= 4;
}

uchar *
qrcode(int ver, int level, int mode, uchar *data, int dlen, int *sidep)
{
	int s, nc, m, mv, mbest, mval, nchar;
	uchar *d, *w, *b;
	uchar verlev;

	qrinit();
	verlev = formsel(ver, level, mode, data, dlen, &nchar);
	if(verlev == 0xff)
		return nil;
	s = side[verlev >> 2];
	d = encode(verlev, data, dlen, mode, nchar);
	if(d == nil)
		return nil;
	w = codewords(verlev, d, &nc);
	free(d);
	if(w == nil)
		return nil;
	b = basepat(verlev);
	if(b == nil){
		free(w);
		return nil;
	}
	fill(verlev, b, w, nc);
	free(w);
	SET(mbest); SET(mval);
	for(m = 0; m < 8; m++){
		mask(verlev, b, m);
		mv = evaluate(verlev, b);
		if(m == 0 || mv < mval){
			mbest = m;
			mval = mv;
		}
		if(m != 7 || mbest != m)
			mask(verlev, b, m);
	}
	if(mbest != 7)
		mask(verlev, b, mbest);
	format(verlev, b, mbest);
	version(verlev, b);
	finalize(verlev, b);
	*sidep = s + 8;
	return b;
}

void
usage(void)
{
	fprint(2, "usage: %s [-LMQHn] [-v ver]\n", argv0);
	exits("usage");
}

void
main(int argc, char **argv)
{
	uchar buf[8192];
	int rc;
	int ver, lev, mode, s;
	char *c;
	uchar *b;
	
	ver = 0;
	lev = 1;
	mode = BYTE;
	ARGBEGIN {
	case 'L': lev = 0; break;
	case 'M': lev = 1; break;
	case 'Q': lev = 2; break;
	case 'H': lev = 3; break;
	case 'v':
		ver = strtol(EARGF(usage()), &c, 0);
		if(*c != 0 || ver < 0 || ver > 40)
			usage();
		break;
	case 'n': mode = NUM; break;
	case 'a': mode = ANUM; break;
	default: usage();
	} ARGEND;
	
	if(argc != 0)
		usage();
	
	rc = readn(0, buf, sizeof(buf));
	if(rc < 0)
		sysfatal("read: %r");
	b = qrcode(ver, lev, mode, buf, rc, &s);
	if(b == nil)
		sysfatal("%r");
	print("%11s %11d %11d %11d %11d ", "k8", 0, 0, s, s);
	write(1, b, s * s);
}

uchar qrrawpolys[] = {
	7, 87, 229, 146, 149, 238, 102, 21,
	0, 10, 251, 67, 46, 61, 118, 70,
	64, 94, 32, 45, 0, 13, 74, 152,
	176, 100, 86, 100, 106, 104, 130, 218,
	206, 140, 78, 0, 15, 8, 183, 61,
	91, 202, 37, 51, 58, 58, 237, 140,
	124, 5, 99, 105, 0, 16, 120, 104,
	107, 109, 102, 161, 76, 3, 91, 191,
	147, 169, 182, 194, 225, 120, 0, 17,
	43, 139, 206, 78, 43, 239, 123, 206,
	214, 147, 24, 99, 150, 39, 243, 163,
	136, 0, 18, 215, 234, 158, 94, 184,
	97, 118, 170, 79, 187, 152, 148, 252,
	179, 5, 98, 96, 153, 0, 20, 17,
	60, 79, 50, 61, 163, 26, 187, 202,
	180, 221, 225, 83, 239, 156, 164, 212,
	212, 188, 190, 0, 22, 210, 171, 247,
	242, 93, 230, 14, 109, 221, 53, 200,
	74, 8, 172, 98, 80, 219, 134, 160,
	105, 165, 231, 0, 24, 229, 121, 135,
	48, 211, 117, 251, 126, 159, 180, 169,
	152, 192, 226, 228, 218, 111, 117, 232,
	87, 96, 227, 21, 0, 26, 173, 125,
	158, 2, 103, 182, 118, 17, 145, 201,
	111, 28, 165, 53, 161, 21, 245, 142,
	13, 102, 48, 227, 153, 145, 218, 70,
	0, 28, 168, 223, 200, 104, 224, 234,
	108, 180, 110, 190, 195, 147, 205, 27,
	232, 201, 21, 43, 245, 87, 42, 195,
	212, 119, 242, 37, 9, 123, 0, 30,
	41, 173, 145, 152, 216, 31, 179, 182,
	50, 48, 110, 86, 239, 96, 222, 125,
	42, 173, 226, 193, 224, 130, 156, 37,
	251, 216, 238, 40, 192, 180, 0, 32,
	10, 6, 106, 190, 249, 167, 4, 67,
	209, 138, 138, 32, 242, 123, 89, 27,
	120, 185, 80, 156, 38, 69, 171, 60,
	28, 222, 80, 52, 254, 185, 220, 241,
	0, 34, 111, 77, 146, 94, 26, 21,
	108, 19, 105, 94, 113, 193, 86, 140,
	163, 125, 58, 158, 229, 239, 218, 103,
	56, 70, 114, 61, 183, 129, 167, 13,
	98, 62, 129, 51, 0, 36, 200, 183,
	98, 16, 172, 31, 246, 234, 60, 152,
	115, 167, 152, 113, 248, 238, 107, 18,
	63, 218, 37, 87, 210, 105, 177, 120,
	74, 121, 196, 117, 251, 113, 233, 30,
	120, 0, 40, 59, 116, 79, 161, 252,
	98, 128, 205, 128, 161, 247, 57, 163,
	56, 235, 106, 53, 26, 187, 174, 226,
	104, 170, 7, 175, 35, 181, 114, 88,
	41, 47, 163, 125, 134, 72, 20, 232,
	53, 35, 15, 0, 42, 250, 103, 221,
	230, 25, 18, 137, 231, 3, 58, 242,
	221, 191, 110, 84, 230, 8, 188, 106,
	96, 147, 15, 131, 139, 34, 101, 223,
	39, 101, 213, 199, 237, 254, 201, 123,
	171, 162, 194, 117, 50, 96, 0, 44,
	190, 7, 61, 121, 71, 246, 69, 55,
	168, 188, 89, 243, 191, 25, 72, 123,
	9, 145, 14, 247, 238, 44, 78, 143,
	62, 224, 126, 118, 114, 68, 163, 52,
	194, 217, 147, 204, 169, 37, 130, 113,
	102, 73, 181, 0, 46, 112, 94, 88,
	112, 253, 224, 202, 115, 187, 99, 89,
	5, 54, 113, 129, 44, 58, 16, 135,
	216, 169, 211, 36, 4, 96, 60, 241,
	73, 104, 234, 8, 249, 245, 119, 174,
	52, 25, 157, 224, 43, 202, 223, 19,
	82, 15, 0, 48, 228, 25, 196, 130,
	211, 146, 60, 24, 251, 90, 39, 102,
	240, 61, 178, 63, 46, 123, 115, 18,
	221, 111, 135, 160, 182, 205, 107, 206,
	95, 150, 120, 184, 91, 21, 247, 156,
	140, 238, 191, 11, 94, 227, 84, 50,
	163, 39, 34, 108, 0, 50, 232, 125,
	157, 161, 164, 9, 118, 46, 209, 99,
	203, 193, 35, 3, 209, 111, 195, 242,
	203, 225, 46, 13, 32, 160, 126, 209,
	130, 160, 242, 215, 242, 75, 77, 42,
	189, 32, 113, 65, 124, 69, 228, 114,
	235, 175, 124, 170, 215, 232, 133, 205,
	0, 52, 116, 50, 86, 186, 50, 220,
	251, 89, 192, 46, 86, 127, 124, 19,
	184, 233, 151, 215, 22, 14, 59, 145,
	37, 242, 203, 134, 254, 89, 190, 94,
	59, 65, 124, 113, 100, 233, 235, 121,
	22, 76, 86, 97, 39, 242, 200, 220,
	101, 33, 239, 254, 116, 51, 0, 54,
	183, 26, 201, 87, 210, 221, 113, 21,
	46, 65, 45, 50, 238, 184, 249, 225,
	102, 58, 209, 218, 109, 165, 26, 95,
	184, 192, 52, 245, 35, 254, 238, 175,
	172, 79, 123, 25, 122, 43, 120, 108,
	215, 80, 128, 201, 235, 8, 153, 59,
	101, 31, 198, 76, 31, 156, 0, 56,
	106, 120, 107, 157, 164, 216, 112, 116,
	2, 91, 248, 163, 36, 201, 202, 229,
	6, 144, 254, 155, 135, 208, 170, 209,
	12, 139, 127, 142, 182, 249, 177, 174,
	190, 28, 10, 85, 239, 184, 101, 124,
	152, 206, 96, 23, 163, 61, 27, 196,
	247, 151, 154, 202, 207, 20, 61, 10,
	0, 58, 82, 116, 26, 247, 66, 27,
	62, 107, 252, 182, 200, 185, 235, 55,
	251, 242, 210, 144, 154, 237, 176, 141,
	192, 248, 152, 249, 206, 85, 253, 142,
	65, 165, 125, 23, 24, 30, 122, 240,
	214, 6, 129, 218, 29, 145, 127, 134,
	206, 245, 117, 29, 41, 63, 159, 142,
	233, 125, 148, 123, 0, 60, 107, 140,
	26, 12, 9, 141, 243, 197, 226, 197,
	219, 45, 211, 101, 219, 120, 28, 181,
	127, 6, 100, 247, 2, 205, 198, 57,
	115, 219, 101, 109, 160, 82, 37, 38,
	238, 49, 160, 209, 121, 86, 11, 124,
	30, 181, 84, 25, 194, 87, 65, 102,
	190, 220, 70, 27, 209, 16, 89, 7,
	33, 240, 0, 62, 65, 202, 113, 98,
	71, 223, 248, 118, 214, 94, 122, 37,
	23, 2, 228, 58, 121, 7, 105, 135,
	78, 243, 118, 70, 76, 223, 89, 72,
	50, 70, 111, 194, 17, 212, 126, 181,
	35, 221, 117, 235, 11, 229, 149, 147,
	123, 213, 40, 115, 6, 200, 100, 26,
	246, 182, 218, 127, 215, 36, 186, 110,
	106, 0, 64, 45, 51, 175, 9, 7,
	158, 159, 49, 68, 119, 92, 123, 177,
	204, 187, 254, 200, 78, 141, 149, 119,
	26, 127, 53, 160, 93, 199, 212, 29,
	24, 145, 156, 208, 150, 218, 209, 4,
	216, 91, 47, 184, 146, 47, 140, 195,
	195, 125, 242, 238, 63, 99, 108, 140,
	230, 242, 31, 204, 11, 178, 243, 217,
	156, 213, 231, 0, 66, 5, 118, 222,
	180, 136, 136, 162, 51, 46, 117, 13,
	215, 81, 17, 139, 247, 197, 171, 95,
	173, 65, 137, 178, 68, 111, 95, 101,
	41, 72, 214, 169, 197, 95, 7, 44,
	154, 77, 111, 236, 40, 121, 143, 63,
	87, 80, 253, 240, 126, 217, 77, 34,
	232, 106, 50, 168, 82, 76, 146, 67,
	106, 171, 25, 132, 93, 45, 105, 0,
	68, 247, 159, 223, 33, 224, 93, 77,
	70, 90, 160, 32, 254, 43, 150, 84,
	101, 190, 205, 133, 52, 60, 202, 165,
	220, 203, 151, 93, 84, 15, 84, 253,
	173, 160, 89, 227, 52, 199, 97, 95,
	231, 52, 177, 41, 125, 137, 241, 166,
	225, 118, 2, 54, 32, 82, 215, 175,
	198, 43, 238, 235, 27, 101, 184, 127,
	3, 5, 8, 163, 238, 0, 0
};

/* (version - 1) * 4 + level, number of blocks, number of bytes in block, number of data bytes */
uchar qrsizes[] = {
	0, 1, 26, 19,
	1, 1, 26, 16,
	2, 1, 26, 13,
	3, 1, 26, 9,
	4, 1, 44, 34,
	5, 1, 44, 28,
	6, 1, 44, 22,
	7, 1, 44, 16,
	8, 1, 70, 55,
	9, 1, 70, 44,
	10, 2, 35, 17,
	11, 2, 35, 13,
	12, 1, 100, 80,
	13, 2, 50, 32,
	14, 2, 50, 24,
	15, 4, 25, 9,
	16, 1, 134, 108,
	17, 2, 67, 43,
	18, 2, 33, 15,
	18, 2, 34, 16,
	19, 2, 33, 11,
	19, 2, 34, 12,
	20, 2, 86, 68,
	21, 4, 43, 27,
	22, 4, 43, 19,
	23, 4, 43, 15,
	24, 2, 98, 78,
	25, 4, 49, 31,
	26, 2, 32, 14,
	26, 4, 33, 15,
	27, 4, 39, 13,
	27, 1, 40, 14,
	28, 2, 121, 97,
	29, 2, 60, 38,
	29, 2, 61, 39,
	30, 4, 40, 18,
	30, 2, 41, 19,
	31, 4, 40, 14,
	31, 2, 41, 15,
	32, 2, 146, 116,
	33, 3, 58, 36,
	33, 2, 59, 37,
	34, 4, 36, 16,
	34, 4, 37, 17,
	35, 4, 36, 12,
	35, 4, 37, 13,
	36, 2, 86, 68,
	36, 2, 87, 69,
	37, 4, 69, 43,
	37, 1, 70, 44,
	38, 6, 43, 19,
	38, 2, 44, 20,
	39, 6, 43, 15,
	39, 2, 44, 16,
	40, 4, 101, 81,
	41, 1, 80, 50,
	41, 4, 81, 51,
	42, 4, 50, 22,
	42, 4, 51, 23,
	43, 3, 36, 12,
	43, 8, 37, 13,
	44, 2, 116, 92,
	44, 2, 117, 93,
	45, 6, 58, 36,
	45, 2, 59, 37,
	46, 4, 46, 20,
	46, 6, 47, 21,
	47, 7, 42, 14,
	47, 4, 43, 15,
	48, 4, 133, 107,
	49, 8, 59, 37,
	49, 1, 60, 38,
	50, 8, 44, 20,
	50, 4, 45, 21,
	51, 12, 33, 11,
	51, 4, 34, 12,
	52, 3, 145, 115,
	52, 1, 146, 116,
	53, 4, 64, 40,
	53, 5, 65, 41,
	54, 11, 36, 16,
	54, 5, 37, 17,
	55, 11, 36, 12,
	55, 5, 37, 13,
	56, 5, 109, 87,
	56, 1, 110, 88,
	57, 5, 65, 41,
	57, 5, 66, 42,
	58, 5, 54, 24,
	58, 7, 55, 25,
	59, 11, 36, 12,
	59, 7, 37, 13,
	60, 5, 122, 98,
	60, 1, 123, 99,
	61, 7, 73, 45,
	61, 3, 74, 46,
	62, 15, 43, 19,
	62, 2, 44, 20,
	63, 3, 45, 15,
	63, 13, 46, 16,
	64, 1, 135, 107,
	64, 5, 136, 108,
	65, 10, 74, 46,
	65, 1, 75, 47,
	66, 1, 50, 22,
	66, 15, 51, 23,
	67, 2, 42, 14,
	67, 17, 43, 15,
	68, 5, 150, 120,
	68, 1, 151, 121,
	69, 9, 69, 43,
	69, 4, 70, 44,
	70, 17, 50, 22,
	70, 1, 51, 23,
	71, 2, 42, 14,
	71, 19, 43, 15,
	72, 3, 141, 113,
	72, 4, 142, 114,
	73, 3, 70, 44,
	73, 11, 71, 45,
	74, 17, 47, 21,
	74, 4, 48, 22,
	75, 9, 39, 13,
	75, 16, 40, 14,
	76, 3, 135, 107,
	76, 5, 136, 108,
	77, 3, 67, 41,
	77, 13, 68, 42,
	78, 15, 54, 24,
	78, 5, 55, 25,
	79, 15, 43, 15,
	79, 10, 44, 16,
	80, 4, 144, 116,
	80, 4, 145, 117,
	81, 17, 68, 42,
	82, 17, 50, 22,
	82, 6, 51, 23,
	83, 19, 46, 16,
	83, 6, 47, 17,
	84, 2, 139, 111,
	84, 7, 140, 112,
	85, 17, 74, 46,
	86, 7, 54, 24,
	86, 16, 55, 25,
	87, 34, 37, 13,
	88, 4, 151, 121,
	88, 5, 152, 122,
	89, 4, 75, 47,
	89, 14, 76, 48,
	90, 11, 54, 24,
	90, 14, 55, 25,
	91, 16, 45, 15,
	91, 14, 46, 16,
	92, 6, 147, 117,
	92, 4, 148, 118,
	93, 6, 73, 45,
	93, 14, 74, 46,
	94, 11, 54, 24,
	94, 16, 55, 25,
	95, 30, 46, 16,
	95, 2, 47, 17,
	96, 8, 132, 106,
	96, 4, 133, 107,
	97, 8, 75, 47,
	97, 13, 76, 48,
	98, 7, 54, 24,
	98, 22, 55, 25,
	99, 22, 45, 15,
	99, 13, 46, 16,
	100, 10, 142, 114,
	100, 2, 143, 115,
	101, 19, 74, 46,
	101, 4, 75, 47,
	102, 28, 50, 22,
	102, 6, 51, 23,
	103, 33, 46, 16,
	103, 4, 47, 17,
	104, 8, 152, 122,
	104, 4, 153, 123,
	105, 22, 73, 45,
	105, 3, 74, 46,
	106, 8, 53, 23,
	106, 26, 54, 24,
	107, 12, 45, 15,
	107, 28, 46, 16,
	108, 3, 147, 117,
	108, 10, 148, 118,
	109, 3, 73, 45,
	109, 23, 74, 46,
	110, 4, 54, 24,
	110, 31, 55, 25,
	111, 11, 45, 15,
	111, 31, 46, 16,
	112, 7, 146, 116,
	112, 7, 147, 117,
	113, 21, 73, 45,
	113, 7, 74, 46,
	114, 1, 53, 23,
	114, 37, 54, 24,
	115, 19, 45, 15,
	115, 26, 46, 16,
	116, 5, 145, 115,
	116, 10, 146, 116,
	117, 19, 75, 47,
	117, 10, 76, 48,
	118, 15, 54, 24,
	118, 25, 55, 25,
	119, 23, 45, 15,
	119, 25, 46, 16,
	120, 13, 145, 115,
	120, 3, 146, 116,
	121, 2, 74, 46,
	121, 29, 75, 47,
	122, 42, 54, 24,
	122, 1, 55, 25,
	123, 23, 45, 15,
	123, 28, 46, 16,
	124, 17, 145, 115,
	125, 10, 74, 46,
	125, 23, 75, 47,
	126, 10, 54, 24,
	126, 35, 55, 25,
	127, 19, 45, 15,
	127, 35, 46, 16,
	128, 17, 145, 115,
	128, 1, 146, 116,
	129, 14, 74, 46,
	129, 21, 75, 47,
	130, 29, 54, 24,
	130, 19, 55, 25,
	131, 11, 45, 15,
	131, 46, 46, 16,
	132, 13, 145, 115,
	132, 6, 146, 116,
	133, 14, 74, 46,
	133, 23, 75, 47,
	134, 44, 54, 24,
	134, 7, 55, 25,
	135, 59, 46, 16,
	135, 1, 47, 17,
	136, 12, 151, 121,
	136, 7, 152, 122,
	137, 12, 75, 47,
	137, 26, 76, 48,
	138, 39, 54, 24,
	138, 14, 55, 25,
	139, 22, 45, 15,
	139, 41, 46, 16,
	140, 6, 151, 121,
	140, 14, 152, 122,
	141, 6, 75, 47,
	141, 34, 76, 48,
	142, 46, 54, 24,
	142, 10, 55, 25,
	143, 2, 45, 15,
	143, 64, 46, 16,
	144, 17, 152, 122,
	144, 4, 153, 123,
	145, 29, 74, 46,
	145, 14, 75, 47,
	146, 49, 54, 24,
	146, 10, 55, 25,
	147, 24, 45, 15,
	147, 46, 46, 16,
	148, 4, 152, 122,
	148, 18, 153, 123,
	149, 13, 74, 46,
	149, 32, 75, 47,
	150, 48, 54, 24,
	150, 14, 55, 25,
	151, 42, 45, 15,
	151, 32, 46, 16,
	152, 20, 147, 117,
	152, 4, 148, 118,
	153, 40, 75, 47,
	153, 7, 76, 48,
	154, 43, 54, 24,
	154, 22, 55, 25,
	155, 10, 45, 15,
	155, 67, 46, 16,
	156, 19, 148, 118,
	156, 6, 149, 119,
	157, 18, 75, 47,
	157, 31, 76, 48,
	158, 34, 54, 24,
	158, 34, 55, 25,
	159, 20, 45, 15,
	159, 61, 46, 16,
	0, 0, 0, 0,
};
