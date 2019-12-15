import asyncio
from sanic import Sanic
from sanic import response
import database as db
import time
from datetime import datetime
import pytz

with open ("template.html", "r") as f:
    __template = f.read()

__srv = Sanic()

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

    recent = db.most_recent()

    if not recent:
        return response.html( __template.format(
            now,'red','never',
            'N/A','N/A','N/A',
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
        now,
        timeout,
        last_report,
        last_temp,
        low_12,
        low_36,
        __tbl_format(day),
        __tbl_format(week),
        __tbl_format(month),
        now - DAY, now - WEEK, now - MONTH, 0
    ))


def __ef(t):
    return '{0:.1f}'.format(t / 128 * 9/5 + 32)

__timezone = pytz.timezone('America/Chicago')
def __if(t):
    d = datetime.fromtimestamp(t, tz=__timezone)
    return d.strftime('%m/%d %H:%M')

def __tbl_format(l):
    o = ''
    max_cnt = 6 * 24 #usual number of records per day
    l_len = len(l)
    for i in range(0, max_cnt+1):
        i = int(round( i / max_cnt * (l_len-1), 0 ))
        if i < l_len:
            ti, te = l[i]
            o += '<tr><td>{}</td><td>{}</td></tr>'.format(__if(ti), __ef(te))
    return o


def setup():
    loop = asyncio.get_event_loop()
    serv_coro = __srv.create_server(host="0.0.0.0", port=8000)
    asyncio.ensure_future(serv_coro, loop=loop)
