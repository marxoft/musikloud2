#!/usr/bin/env python

# Copyright (C) 2015 Stuart Howarth <showarth@marxoft.co.uk>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 3 as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

from datetime import datetime
import getopt
import re
import sys
import urllib2
import xml.etree.ElementTree as elemtree

try:
    import json
except ImportError:
    import simplejson as json

ACCESS_TOKEN = 'ZjdjOTlmMDI4NGE0OWNkMzgxMzIzZTZiYTM4YmM2OTQ3NzgwMzE1OToK'

API_URL = 'https://cuteradio.herokuapp.com'
COUNTRIES_URL = API_URL + '/countries'
GENRES_URL = API_URL + '/genres'
LANGUAGES_URL = API_URL + '/languages'
STATIONS_URL = API_URL + '/stations'

PLAYLIST_EXTS = ['asx', 'm3u', 'pls', 'ram', 'smil']

MAX_RESULTS = 20
MAX_RETRIES = 8

class ResourceError(Exception):
    pass
    
def format_date(date_string, date_format = '%a, %d %b %Y %H:%M:%S %Z'):
    try:
        return datetime.strptime(date_string, date_format).strftime('%d %b %Y')
    except:
        return date_string

def get_response(url, token_required = False):
    try:        
        if token_required:
            url = re.sub(r'&(?![\w_]+=)', '%26', url).replace(' ', '%20').replace('\\"', '"')
            request = urllib2.Request(url)
            request.add_header('Content-Type', 'application/json')
            request.add_header('Accept', 'application/json')
            request.add_header('Authorization', 'Basic ' + ACCESS_TOKEN)
            return urllib2.urlopen(request).read()
        else:
            return urllib2.urlopen(url).read()
    except:
        raise ResourceError('{"error": "Unable to retrieve data from %s"}' % url)

def get_playlist_ext(url):
    try:
        ext = url.split('.')[-1].lower()
        
        if ext in PLAYLIST_EXTS:
            return ext
    except IndexError:
        pass
    
    return None

def url_is_playlist(url):
    return get_playlist_ext(url) is not None

def get_stream_url_from_unknown_file(response):
    try:
        return re.search(r'(http(s|)|mms)://[^\s\"<>]+', response).group(0)
    except:
        return None

def get_stream_url_from_asx_file(response):    
    try:
        return elemtree.fromstring(response.lower()).find('entry').find('entryref').attrib['href']
    except:
        return get_stream_from_unknown_file(response)

def get_stream_url_from_m3u_file(response):
    try:
        return re.search(r'(http(s|)|mms)://\S+', response).group(0)
    except:
        return get_stream_url_from_unknown_file(response)

def get_stream_url_from_pls_file(response):
    try:
        return re.search(r'(?<=File\d=)\S+', response).group(0)
    except:
        return get_stream_url_from_unknown_file(response)

def get_stream_url_from_smil_file(response):    
    try:
        return elemtree.fromstring(response.lower()).find('body').find('audio').attrib['src']
    except:
        return get_stream_from_unknown_file(response)

def get_stream_url(url):
    retries = 0
    
    ext = get_playlist_ext(url)
    
    while ext is not None and retries < MAX_RETRIES:
        response = get_response(url)
        
        if ext == 'asx':
            url = get_stream_url_from_asx_file(response)
        elif ext == 'm3u':
            url = get_stream_url_from_m3u_file(response)
        elif ext == 'pls':
            url = get_stream_url_from_pls_file(response)
        elif ext == 'ram':
            url = get_stream_url_from_ram_file(response)
        elif ext == 'smil':
            url = get_stream_url_from_smil_file(response)
        
        ext = get_playlist_ext(url)
        retries += 1
    
    return url

def list_tracks(url):
    if not url:
        raise ResourceError('{"error": "No URL specified"}')
        
    try:
        if url[0:8] != 'https://':
            # Get related stations
            url = '%s?where={"genre":"%s"}&max_results=%d' % (STATIONS_URL, get_track(url)['genre'], MAX_RESULTS)
        
        response = json.loads(get_response(url, True))
    except:
        raise ResourceError('{"error": "Unable to parse response from %s"}' % url)
    
    result = {}
    tracks = []
    
    for item in response['items']:
        try:
            track = {}
            track['date'] = format_date(item['created'])
            track['description'] = item['description']
            track['downloadable'] = False
            track['duration'] = 0
            track['genre'] = item['genre']
            track['id'] = item['id']
            track['title'] = item['title']
            track['url'] = item['source']
        
            if not url_is_playlist(item['source']):
                track['streamUrl'] = item['source']
        
            tracks.append(track)
        except:
            pass
    
    result['items'] = tracks
    
    try:
        result['next'] = 'https://' + response['links']['next']['href']
    except:
        pass
    
    return result

def search_tracks(query, order):
    return list_tracks('%s?where={"title":{"$regex":"%s","$options":"i"}}&max_results=%d' % (STATIONS_URL, query, MAX_RESULTS))

def get_track(id):
    if not id:
        raise ResourceError('{"error": "No ID specified"}')
        
    try:
        item = json.loads(get_response(STATIONS_URL + '/' + id, True))
        track = {}
        track['date'] = format_date(item['created'])
        track['description'] = item['description']
        track['downloadable'] = False
        track['duration'] = 0
        track['genre'] = item['genre']
        track['id'] = STATIONS_URL + '/' + item['id']
        track['title'] = item['title']
        track['url'] = item['source']

        if not url_is_playlist(item['source']):
            track['streamUrl'] = item['source']
        
        return track
    except:
        raise ResourceError('{"error": "No station found"}')

def list_categories(url):
    if not url:
        raise ResourceError('{"error": "No URL specified"}')
        
    try:
        response = json.loads(get_response(url, True))
    except:
        raise ResourceError('{"error": "Unable to parse response from %s"}' % url)
    
    result = {}
    categories = []
    category_type = 'genre'
    
    try:
        path = url.split('?')[0].split('/')[-1]
    
        if path == 'countries':
            category_type = 'country'
        elif path == 'languages':
            category_type = 'language'
    except:
        pass
    
    for item in response['items']:
        try:
            category = {}
            category['id'] = '%s?where={"%s":"%s"}&max_results=%d' % (STATIONS_URL, category_type, item['name'], MAX_RESULTS)
            category['title'] = item['name']
            categories.append(category)
        except:
            pass
    
    result['items'] = categories
    
    try:
        result['next'] = 'https://' + response['links']['next']['href']
    except:
        pass
    
    return result

def list_streams(id):
    if not id:
        raise ResourceError('{"error": "No ID specified"}')
    
    result = {}
    streams = []
    
    try:
        stream = {}
        stream['description'] = 'Live radio stream'
        stream['id'] = '0'
        stream['url'] = get_stream_url(get_track(id)['url'])
        streams.append(stream)
    except:
        pass
    
    result['items'] = streams
    return result
        
def list_items(resource, id):
    if not resource or resource == 'track':
        return list_tracks(id)
    
    if resource == 'category':
        return list_categories(id)
    
    if resource == 'stream':
        return list_streams(id)
    
    return []
        
def search_items(resoruce, query, order):
    if not resource or resource == 'track':
        return search_tracks(query, order)
    
    return []

def get_item(resource, id):
    if not resource or resource == 'track':
        return get_track(id)
    
    return {}
        
def main(method, resource, id, query, order):
    if method == 'list':
        print json.dumps(list_items(resource, id))
    elif method == 'search':
        print json.dumps(search_items(resource, query, order))
    elif method == 'get':
        print json.dumps(get_item(resource, id))
    else:
        raise ResourceError('{"error": "Invalid method specified: %s"}' % method)

if __name__ == '__main__':
    (opts, args) = getopt.getopt(sys.argv[1:], 'm:r:i:q:o:')
    
    method = 'list'
    resource = 'track'
    id = ''
    query = ''
    order = ''
    
    for o, a in opts:
        if o == '-m':
            method = a
        elif o == '-r':
            resource = a
        elif o == '-i':
            id = a
        elif o == '-q':
            query = a
        elif o == '-o':
            order = a
    
    try:
        main(method, resource, id, query, order)
        exit(0)
    except ResourceError, e:
        print e.args[0]
        exit(1)
