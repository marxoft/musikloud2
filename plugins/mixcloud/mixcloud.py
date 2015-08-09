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

try:
    import json
except ImportError:
    import simplejson as json

API_URL = 'https://api.mixcloud.com'

class ResourceError(Exception):
    pass

def check_url(url):
    try:
        request = urllib2.Request(url)
        request.get_method = lambda : 'HEAD'
        urllib2.urlopen(request)
        return True
    except:
        return False

def format_date(date_string, date_format = '%Y-%m-%dT%H:%M:%SZ'):    
    try:
        return datetime.strptime(date_string, date_format).strftime('%d %b %Y')
    except:
        return date_string

def get_page(url):
    try:
        return urllib2.urlopen(url).read()
    except:
        raise ResourceError('{"error": "Unable to retrieve page from %s"}' % url)

def list_tracks(url):
    if not url:
        raise ResourceError('{"error": "No URL specified"}')
        
    try:
        response = json.loads(get_page(url.replace('www.mixcloud.com', 'api.mixcloud.com')))
    except:
        raise ResourceError('{"error": "Unable to parse response from %s"}' % url)
    
    result = {}
    tracks = []
    
    try:
        for item in response['data']:
            try:
                track = {}
                track['artist'] = item['user']['name']
                track['artistId'] = API_URL + item['user']['key']
                track['date'] = format_date(item['created_time'])
                track['duration'] = item['audio_length'] * 1000
        
                try:
                    track['genre'] = item['tags'][0]['name']
                except IndexError:
                    pass
        
                track['id'] = item['url']
                track['largeThumbnailUrl'] = item['pictures']['large']
                track['thumbnailUrl'] = item['pictures']['medium']
                track['title'] = item['name']
                track['url'] = item['url']
                tracks.append(track)
            except:
                pass
    except KeyError:
        pass
    
    result['items'] = tracks
    
    try:
        result['next'] = response['paging']['next']
    except:
        pass
    
    return result

def search_tracks(query, order):
    return list_tracks('%s/search/?q=%s&type=cloudcast' % (API_URL, query.replace(' ', '+')))

def get_track(url):
    if not url:
        raise ResourceError('{"error": "No URL specified"}')
    
    try:
        response = json.loads(get_page(url.replace('www.mixcloud.com', 'api.mixcloud.com')))
    except:
        raise ResourceError('{"error": "Unable to parse response from %s"}' % url)
    
    try:
        track = {}
        track['artist'] = response['user']['name']
        track['artistId'] = API_URL + response['user']['key']
        track['date'] = format_date(response['created_time'])
        track['duration'] = response['audio_length'] * 1000
    
        try:
            track['genre'] = item['tags'][0]['name']
        except IndexError:
            pass
    
        track['id'] = response['url']
        track['largeThumbnailUrl'] = response['pictures']['large']
        track['thumbnailUrl'] = response['pictures']['medium']
        track['title'] = response['name']
        track['url'] = response['url']
        return track
    except:
        raise ResourceError('{"error": "Cloudcast not found"}')

def list_artists(url):
    if not url:
        raise ResourceError('{"error": "No URL specified"}')
        
    try:
        response = json.loads(get_page(url.replace('www.mixcloud.com', 'api.mixcloud.com')))
    except:
        raise ResourceError('{"error": "Unable to parse response from %s"}' % url)
    
    result = {}
    artists = []
    
    for item in response['data']:
        try:
            artist = {}
            artist['id'] = API_URL + item['key'] + 'cloudcasts/'
            artist['largeThumbnailUrl'] = item['pictures']['large']
            artist['name'] = item['name']
            artist['thumbnailUrl'] = item['pictures']['medium']
            artists.append(artist)
        except:
            pass
    
    result['items'] = tracks
    
    try:
        result['next'] = response['paging']['next']
    except:
        pass
    
    return result

def search_artists(query, order):
    return list_artists('%s/search/?q=%s&type=user' % (API_URL, query.replace(' ', '+')))

def get_artist(url):
    if not url:
        raise ResourceError('{"error": "No URL specified"}')
    
    try:
        response = json.loads(get_page(url.replace('www.mixcloud.com', 'api.mixcloud.com')))
    except:
        raise ResourceError('{"error": "Unable to parse response from %s"}' % url)
    
    try:
        artist = {}
        artist['id'] = API_URL + response['key'] + 'cloudcasts/'
        artist['largeThumbnailUrl'] = response['pictures']['large']
        artist['name'] = response['name']
        artist['thumbnailUrl'] = response['pictures']['medium']
        return artist
    except:
        raise ResourceError('{"error": "User not found"}')

def list_categories(url):
    if not url:
        raise ResourceError('{"error": "No URL specified"}')
        
    try:
        response = json.loads(get_page(url.replace('www.mixcloud.com', 'api.mixcloud.com')))
    except:
        raise ResourceError('{"error": "Unable to parse response from %s"}' % url)
    
    result = {}
    categories = []
    
    for item in response['data']:
        try:
            category = {}
            category['id'] = API_URL + item['key'] + 'cloudcasts/'
            category['title'] = item['name']
            categories.append(category)
        except:
            pass
    
    result['items'] = categories
    
    try:
        result['next'] = response['paging']['next']
    except:
        pass
    
    return result

def list_streams(url):
    if not url:
        raise ResourceError('{"error": "No URL specified"}')
    
    page = get_page(url.replace('api.mixcloud.com', 'www.mixcloud.com'))
    result = {}
    streams = []
    
    try:
        preview_url = re.search(r'(?<=m-preview=")[^"]+', page).group(0)
        stream_url = preview_url.replace('https://', 'http://').replace('/previews/', '/c/originals/')
    
        if check_url(stream_url):
            stream = {}
            stream['description'] = 'MP3'
            stream['ext'] = 'mp3'
            stream['id'] = '0'
            stream['url'] = stream_url
            streams.append(stream)
        else:
            stream_url = stream_url.replace('.mp3', '.m4a').replace('originals/', 'm4a/64/')
        
            if check_url(stream_url):
                stream = {}
                stream['description'] = 'M4A'
                stream['ext'] = 'm4a'
                stream['id'] = '0'
                stream['url'] = stream_url
                streams.append(stream)
    except:
        pass
    
    result['items'] = streams
    return result

def get_item(resource, id):
    if resource == 'track':
        return get_track(id)
    
    if resource == 'artist':
        return get_artist(id)
    
    return {}
        
def list_items(resource, id):
    if not resource or resource == 'track':
        return list_tracks(id)
    
    if resource == 'artist':
        return list_artists(id)
    
    if resource == 'category':
        return list_categories(id)
    
    if resource == 'stream':
        return list_streams(id)
    
    return []
        
def search_items(resoruce, query, order):
    if not resource or resource == 'track':
        return search_tracks(query, order)
    
    if resource == 'artist':
        return search_artists(query, order)
    
    return []
        
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
