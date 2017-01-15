/*
 * Copyright (C) 2016 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

var API_URL = "https://api.mixcloud.com";
var WEB_URL = "https://www.mixcloud.com";
var USER_AGENT = "Wget/1.13.4 (linux-gnu)";
var STREAMS_COOKIE = "s=3lu41hy4i1seitbhbtqkt5x5v94wwziy";

var request = null;

function formatDate(iso) {
    try {
        var date = iso.substring(0, iso.indexOf("T")).split("-");
        return new Date(date[0], date[1] - 1, date[2]).toLocaleDateString();
    }
    catch(err) {
        return null;
    }
}

function artistToItem(artist) {
    var item = {};
    item["description"] = artist.biog;
    item["largeThumbnailUrl"] = artist.pictures.large;
    item["name"] = artist.name;
    item["thumbnailUrl"] = artist.pictures.medium;
    item["tracksId"] = artist.key + "cloudcasts/";
    item["url"] = artist.url;
    return item;
}

function cloudcastToItem(cloudcast) {
    var item = {};
    item["artist"] = cloudcast.user.name;
    item["artistId"] = cloudcast.user.key;
    item["commentsId"] = cloudcast.key + "comments/";
    item["date"] = formatDate(cloudcast.created_time);
    item["description"] = cloudcast.description;
    item["duration"] = cloudcast.audio_length * 1000;
    item["id"] = cloudcast.key;
    item["largeThumbnailUrl"] = cloudcast.pictures.large;
    item["playCount"] = cloudcast.listener_count;
    item["relatedTracksId"] = cloudcast.user.key + "cloudcasts/";
    item["thumbnailUrl"] = cloudcast.pictures.medium;
    item["title"] = cloudcast.name;
    item["url"] = cloudcast.url;
    
    try {
        item["genre"] = cloudcast.tags[0].name;
    }
    catch(err) {}
    
    return item;
}

function categoryToItem(category) {
    var item = {};
    item["id"] = category.key;
    item["title"] = category.name;
    item["tracksId"] = category.key + "cloudcasts/";
    return item;
}

function commentToItem(comment) {
    var item = {};
    item["artist"] = comment.user.name;
    item["artistId"] = comment.user.key;
    item["body"] = comment.comment;
    item["date"] = formatDate(comment.submit_date);
    item["id"] = comment.key;
    item["thumbnailUrl"] = comment.user.pictures.medium;
    item["url"] = comment.url;
    return item;
}

function getArtist(url) {
    if (url.indexOf("/") === 0) {
        url = API_URL + url;
    }
    else {
        url = url.replace("www.mixcloud.com", "api.mixcloud.com");
    }
    
    request = new XMLHttpRequest();
    request.onreadystatechange = function() {
        if (request.readyState === 4) {
            try {
                var response = JSON.parse(request.responseText);            
                finished(artistToItem(response));
            }
            catch(err) {
                error(err);
            }
        }
    }
    
    request.open("GET", url);
    request.setRequestHeader("User-Agent", USER_AGENT);
    request.send();
}

function listCloudcasts(url) {
    if (url.indexOf("/") === 0) {
        url = API_URL + url;
    }
    else {
        url = url.replace("www.mixcloud.com", "api.mixcloud.com");
    }
    
    request = new XMLHttpRequest();
    request.onreadystatechange = function() {
        if (request.readyState === 4) {
            try {
                var response = JSON.parse(request.responseText);
                var result = {};
                var items = [];
                
                for (var i = 0; i < response.data.length; i++) {
                    items.push(cloudcastToItem(response.data[i]));
                }
                
                result["items"] = items;
                
                try {
                    result["next"] = response.paging.next;
                }
                catch(err) {}
                
                finished(result);
            }
            catch(err) {
                error(err);
            }
        }
    }
    
    request.open("GET", url);
    request.setRequestHeader("User-Agent", USER_AGENT);
    request.send();
}

function searchCloudcasts(query) {
    listCloudcasts(API_URL + "/search/?q=" + query + "&type=cloudcast");
}

function getCloudcast(url) {
    url = url.replace("www.mixcloud.com", "api.mixcloud.com");
    request = new XMLHttpRequest();
    request.onreadystatechange = function() {
        if (request.readyState === 4) {
            try {
                var response = JSON.parse(request.responseText);            
                finished(cloudcastToItem(response));
            }
            catch(err) {
                error(err);
            }
        }
    }
    
    request.open("GET", url);
    request.setRequestHeader("User-Agent", USER_AGENT);
    request.send();
}

function listCategories(url) {
    if (url.indexOf("/") === 0) {
        url = API_URL + url;
    }
    else {
        url = url.replace("www.mixcloud.com", "api.mixcloud.com");
    }
    
    request = new XMLHttpRequest();
    request.onreadystatechange = function() {
        if (request.readyState === 4) {
            try {
                var response = JSON.parse(request.responseText);
                var result = {};
                var items = [];
                
                for (var i = 0; i < response.data.length; i++) {
                    items.push(categoryToItem(response.data[i]));
                }
                
                result["items"] = items;
                finished(result);
            }
            catch(err) {
                error(err);
            }
        }
    }
    
    request.open("GET", url);
    request.setRequestHeader("User-Agent", USER_AGENT);
    request.send();
}

function listComments(url) {
    if (url.indexOf("/") === 0) {
        url = API_URL + url;
    }
    else {
        url = url.replace("www.mixcloud.com", "api.mixcloud.com");
    }
    
    request = new XMLHttpRequest();
    request.onreadystatechange = function() {
        if (request.readyState === 4) {
            try {
                var response = JSON.parse(request.responseText);
                var result = {};
                var items = [];
                
                for (var i = 0; i < response.data.length; i++) {
                    items.push(commentToItem(response.data[i]));
                }
                
                result["items"] = items;
                
                try {
                    result["next"] = response.paging.next;
                }
                catch(err) {}
                
                finished(result);
            }
            catch(err) {
                error(err);
            }
        }
    }
    
    request.open("GET", url);
    request.setRequestHeader("User-Agent", USER_AGENT);
    request.send();
}

function listStreams(url) {
    if (url.indexOf("/") === 0) {
        url = WEB_URL + url;
    }
    else {
        url = url.replace("api.mixcloud.com", "www.mixcloud.com");
    }
    
    request = new XMLHttpRequest();
    request.onreadystatechange = function() {
        if (request.readyState === 4) {
            try {
                var result = {};
                var items = [];
                var response = request.responseText;
                var previewUrl = /m-preview="([^"]+)/.exec(response)[1];
                var streamHost = /stream(\d+)\.mixcloud\.com/.exec(response)[0];
                var streamUrl = "http://" + streamHost + previewUrl.split("mixcloud.com")[1]
                    .replace("/previews/", "/c/m4a/64/").replace(/\.mp3$/, ".m4a");
                var item = {};
                item["description"] = "M4A";
                item["ext"] = "m4a";
                item["id"] = "m4a";
                item["url"] = streamUrl;
                items.push(item);
                result["items"] = items;
                finished(result);
            }
            catch(err) {
                error(err);
            }
        }
    }
    
    request.open("GET", url);
    request.setRequestHeader("User-Agent", USER_AGENT);
    request.setRequestHeader("Cookie", STREAMS_COOKIE);
    request.send();
}

function get(resource, id) {
    switch (resource) {
    case "artist":
        getArtist(id);
        return true;
    case "track":
        getCloudcast(id);
        return true;
    default:
        return false;
    }
}

function list(resource, id) {
    switch (resource) {
    case "track":
        listCloudcasts(id);
        return true;
    case "category":
        listCategories(id);
        return true;
    case "comment":
        listComments(id);
        return true;
    case "stream":
        listStreams(id);
        return true;
    default:
        return false;
    }
}

function search(resource, query, order) {
    switch (resource) {
    case "track":
        searchCloudcasts(query);
        return true;
    default:
        return false;
    }
}

function cancel() {
    if (request) {
        request.abort();
    }
    
    return true;
}
