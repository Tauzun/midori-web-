import { observable, computed } from 'mobx';
import { ISettings, ITheme, IVisitedItem } from '~/interfaces';
import { getTheme } from '~/utils/themes';
import { requestURL } from '~/utils/network';
import { INewsItem } from '~/interfaces/news-item';

export class Store {
  @observable
  public settings: ISettings = { ...(window as any).settings };

  @computed
  public get theme(): ITheme {
    return getTheme(this.settings.theme);
  }

  @observable
  public news: INewsItem[] = [];

  @observable
  public image = '';

  private page = 1;
  private loaded = true;

  @observable
  public topSites: IVisitedItem[] = [];

  public constructor() {
    (window as any).updateSettings = (settings: ISettings) => {
      this.settings = { ...this.settings, ...settings };
    };

    this.loadImage();
    this.loadTopSites();
  }

  public async loadImage() {
    let url = 'https://picsum.photos/1920/1080';
    let isNewUrl = true;
    const dateString = localStorage.getItem('imageDate');

    if (dateString && dateString !== '') {
      const date = new Date(dateString);
      const date2 = new Date();
      const diffTime = Math.floor(
        (date2.getTime() - date.getTime()) / (1000 * 60 * 60 * 24),
      );
      const newUrl = localStorage.getItem('imageURL');

      if (diffTime < 1 && newUrl) {
        url = newUrl;
        isNewUrl = false;
      }
    }

    fetch(url)
      .then(response => Promise.all([response.url, response.blob()]))
      .then(([resource, blob]) => {
        this.image = URL.createObjectURL(blob);

        return resource;
      })
      .then(imgUrl => {
        if (isNewUrl) {
          localStorage.setItem('imageURL', imgUrl);
          localStorage.setItem('imageDate', new Date().toString());
        }
      })
      .catch(e => console.error(e));
  }

  public async loadTopSites() {
    this.topSites = await (window as any).getTopSites(8);
  }
}

export default new Store();
