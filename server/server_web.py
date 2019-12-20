import asyncio
from sanic import Sanic
from sanic import response
import database as db
import time
from datetime import datetime
import pytz
from math import floor, ceil
from config import WEB_PORT

with open ("template.html", "r") as f:
    __template = f.read()

__srv = Sanic()

@__srv.route('/isup', methods=['GET', 'HEAD'])
async def __isup(req):
    recent = db.most_recent()
    if not recent or time.time() - recent[0] > 60 * 45:
        return response.text('sensor timeout\n', status=503 )
    else:
        return response.text('sensor is reporting\n', status=200 )

@__srv.route('/csv')
async def __dl(req):
    try:
        t = int(req.raw_args['t'])
    except:
        return response.text( 'missing or invalid query param \'t\'', status=400 )
    return response.text(
        '\n'.join([  ','.join([str(i) for i in r]) for r in db.iterate_recent(time_cutoff = t)  ])
        )

@__srv.route('/')
async def __root(req):
    now = int(time.time())
    HOUR = 60 * 60
    DAY = HOUR * 24
    WEEK = DAY * 7
    MONTH = DAY * 31
    now_f = __if( now, '%m/%d at %H:%M')

    recent = db.most_recent()

    if not recent:
        return response.html( __template.format(
            now_f,'red','never',
            'N/A','N/A','N/A',
            '',
            '','','',
            now - DAY, now - WEEK, now - MONTH, 0
        ))

    last_report = recent[0]

    timeout = 'red' if now - last_report > 60 * 45 else '' #45 minute timeout
    last_report = int( (now - last_report) / 60 ) #convert to differental minutes
    if last_report > WEEK * 1.5:
        last_report = 'a very long time ago'
    else:
        last_report = '{} minute{} ago'.format(last_report, 's' if last_report != 1 else '')
    last_temp = __ef(recent[1])
    low_12 = db.min_temp(now - HOUR * 12)
    low_36 = db.min_temp(now - HOUR * 36)
    low_12 = 'NA' if not low_12 else __ef(low_12)
    low_36 = 'NA' if not low_36 else __ef(low_36)

    tbl_day = ''
    tbl_week = ''
    tbl_month = ''

    day = list( db.iterate_recent(time_cutoff = now - DAY) )
    week = list( db.iterate_recent(time_cutoff = now - WEEK) )
    month = list( db.iterate_recent(time_cutoff = now - MONTH) )


    return response.html( __template.format(
        now_f,
        timeout,
        last_report,
        last_temp,
        low_12,
        low_36,
        __make_chart(HOUR * 36),
        __tbl_format(day),
        __tbl_format(week),
        __tbl_format(month),
        now - DAY, now - WEEK, now - MONTH, 0
    ))


def __efi(t):
    return t / 128 * 9/5 + 32

def __ef(t):
    return '{:.1f}'.format(__efi(t))

__timezone = pytz.timezone('America/Chicago')
def __if(t, f='%m/%d %H:%M'):
    d = datetime.fromtimestamp(t, tz=__timezone)
    return d.strftime(f)

def __tbl_format(l):
    o = ''
    max_cnt = 6 * 24 #usual number of records per day
    l_len = len(l)
    idx_prev = -1
    for idx in range(0, max_cnt+1):
        idx = int(round( idx / max_cnt * (l_len-1), 0 ))
        if idx < l_len and idx != idx_prev:
            idx_prev = idx
            ti, te = l[idx]
            o += '<tr><td>{}</td><td>{}</td></tr>'.format(__if(ti), __ef(te))
    return o

def __make_chart(window):
    BASE = 1000
    RATIO = 4/1
    def linmap(value, leftMin, leftMax, rightMin, rightMax):
        return rightMin + float(value - leftMin) / float(leftMax - leftMin) * (rightMax - rightMin)

    pts = list( db.iterate_recent(time_cutoff = time.time() - window) )
    if len(pts) < 2: return ''

    pts = [[p[0], __efi(p[1])] for p in pts]
    pts_rot = list(zip(*pts))
    min_i, min_e = map(min, pts_rot)
    max_i, max_e = map(max, pts_rot)
    min_e = floor(min_e)
    max_e = ceil(max_e)

    elems = []

    for l in range(int(min_e), int(max_e) + 1, 1):
        elems.append( '<path d="M 0 {0:.2f} L {1:.2f} {0:.2f}" stroke="grey" stroke-width="2"/>'.format(linmap(l, min_e,max_e, 0,BASE), BASE*RATIO) )

    path = []
    for p in pts:
        i, e = p
        e = linmap(e, min_e, max_e, max_e, min_e)
        path.append( '{:.2f} {:.2f}'.format( linmap(i, min_i,max_i, 0,BASE*RATIO), linmap(e, min_e,max_e, 0,BASE)) )
    elems.append( '<path d="M ' + ' L '.join(path) + '" stroke="black" fill="none" stroke-width="6"/>' )

    return '<svg viewBox="0 0 {} {}" width="100%">{}</svg>'.format(int(BASE*RATIO), int(BASE), '\n'.join(elems) )

def setup():
    loop = asyncio.get_event_loop()
    serv_coro = __srv.create_server(host="0.0.0.0", port=WEB_PORT, return_asyncio_server=True)
    serv_task = asyncio.ensure_future(serv_coro, loop=loop)
    server = loop.run_until_complete(serv_task)
    server.after_start()
